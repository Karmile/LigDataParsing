#include "WorkThreads.h"
#include <future>
#include "include/ordered_map.h"

using namespace tsl;
void ProcessCombinationPool(vector<vector<TriggerInfo>>& locCombinationPool, ordered_map<int, triggerAtStation>& triggerPool, double& MinSq, LocSta& result, __int64& finalCombIdx, unsigned long long& CountGeoLocationTimes)
{
	double* sqList = new double[locCombinationPool.size()];
	LocSta* resultList = new LocSta[locCombinationPool.size()];

	int num_threads = locCombinationPool.size();
#pragma omp parallel for num_threads(locCombinationPool.size())
	for (int m = 0; m < locCombinationPool.size(); ++m)
	{
		vector<TriggerInfo>& oneComb = locCombinationPool[m];
		/*			if (oneComb[0].stationID != baseTrig.stationID) {
						sqList[m] = FLOAT_MAX;
						continue;
					}*/
		if (oneComb.size() < 5)
		{
			sqList[m] = FLOAT_MAX;
			continue;
		}
		vector<double> Loc_Time_One;
		vector<LocSta> Stations_One;

		for (int j = 0; j < oneComb.size(); ++j)
		{
			Loc_Time_One.emplace_back(oneComb[j].time.m_Sec + oneComb[j].time.m_ActPointSec);
			Stations_One.emplace_back(triggerPool[oneComb[j].stationID].staLocation);
		}

		LocSta oneResult = GeoLocation_GPU(Stations_One, Loc_Time_One);
		//LocSta oneResult = GeoLocation_GPU_Initial(Stations_One, Loc_Time_One);
		//LocSta oneResult = GeoLocation_OP(Stations_One, Loc_Time_One, LocSta(0.0, 0.0, 0.0), num_threads);
		//LocSta oneResult = GeoLocation_OP_2(Stations_One, Loc_Time_One, LocSta(0.0, 0.0, 0.0), num_threads);

		sqList[m] = oneResult.sq;
		resultList[m] = oneResult;
	}
	// 查找最小值
	CountGeoLocationTimes += locCombinationPool.size();
	auto minElementPtr = min_element(sqList, sqList + locCombinationPool.size());

	//if (MinSq > *minElementPtr)
	{
		MinSq = *minElementPtr;
		finalCombIdx = minElementPtr - sqList;
		result = resultList[finalCombIdx];
	}

	delete[] sqList;
	delete[] resultList;
}

void ThreadLoc(deque<TriggerInfo>& allTriggers, deque<TriggerInfo>& transTriggers, unordered_map<int, StationInfo>& siteMap, unordered_map<int, unordered_map<int, double>>& siteTimeMap, shared_mutex& rwMutex, YAML::Node config)
{
	ThreadPool postThreadPool{ 100 };
	double maxBaseLineAsTOA = (config["maxBaseLineAsTOA"].as<double>());
	double LocThresholdInitial = config["LocThresholdInitial"].as<double>();
	double checkTheta = config["checkTheta"].as<double>();
	double waitTime = config["waitTime"].as<double>();
	double LocThresholdFinal = config["LocThresholdFinal"].as<double>();
	GPSTime CurrentProcessingTime = GPSTime();

	// 用于测试
	while (1)
	{
		//合并数据
		if (transTriggers.size())
		{
			unique_lock<shared_mutex> lock(rwMutex);

			for (const auto& trigger : transTriggers) {
				if (trigger.time > CurrentProcessingTime)
					allTriggers.push_back(trigger);
			}
			sort(allTriggers.begin(), allTriggers.end()); // 按照时间排序
			allTriggers.erase(unique(allTriggers.begin(), allTriggers.end()), allTriggers.end()); // 去重

			transTriggers.clear();
			lock.unlock();
		}
		else
		{
			cout << "Wait for new trigger data... 10s" << endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
			continue;
		}

		cout << "New data merge and begin processing: " << CGPSTimeAlgorithm::GetTimeString(CurrentProcessingTime) << endl;
		cout << "Size of allTriggers: " << allTriggers.size() << endl;

		auto start = std::chrono::high_resolution_clock::now();

		ofstream outfile_O;
		outfile_O.open("lig_txt/NewData2.txt", ios::out);
		// 开始定位

		while (allTriggers.size())
		{
			// Wait time
			if (allTriggers.size() > 1) {
				if (allTriggers[1].time - allTriggers[0].time > maxBaseLineAsTOA)
				{
					allTriggers.erase(allTriggers.begin());
					continue;
				}
			}

			ordered_map<int, triggerAtStation> triggerPool;
			TriggerInfo& baseTrig = allTriggers[0];
			//vector<int> recycleIdx;

			// 预留网络等待的时间，如果不满足则直接返回
			CurrentProcessingTime = CGPSTimeAlgorithm::AddActPointSec(baseTrig.time, maxBaseLineAsTOA);
			if ((CGPSTimeAlgorithm::ConvertGPSTimeToUnixTime(baseTrig.time) + 8 * 3600) > (time(nullptr) - waitTime)) break;

			//cout << "Time: " << (CGPSTimeAlgorithm::ConvertGPSTimeToUnixTime(baseTrig.time) + 8 * 3600) << " " << (time(nullptr) - waitTime) << endl;
			// CurrentProcessingTime = allTriggers[0].time;

			for (int j = 0; j < allTriggers.size(); ++j)
			{
				TriggerInfo& oneTrig = allTriggers[j];
				if (baseTrig.Value * oneTrig.Value < 0) continue;

				int trigSiteID = oneTrig.stationID;
				double diffTime = baseTrig.time - oneTrig.time;

				if ((diffTime < -maxBaseLineAsTOA) || (diffTime > 0)) break;

				// 这里第一个基站只放入一个数据，因为筛选标准是以第一个基站定标的，所以第一个基站只放入一个数据
				if ((fabs(diffTime) <= (siteTimeMap[baseTrig.stationID][oneTrig.stationID]+0.0001))) // || (baseTrig.stationID == oneTrig.stationID))
				{
					// 判断键值是否存在
					if (triggerPool.find(trigSiteID) == triggerPool.end())
					{
						if (triggerPool.size() >= 8) continue; // New stategy, 使用距离最近的8个站点定位

						//recycleIdx.push_back(j);
						triggerAtStation oneTriggerAtStation;
						oneTriggerAtStation.staLocation = (LocSta(siteMap[trigSiteID].latitude * degree2radians,
							siteMap[trigSiteID].longitude * degree2radians,
							siteMap[trigSiteID].altitude / 1000.0));
						oneTriggerAtStation.stationID = trigSiteID;
						oneTriggerAtStation.triggers.emplace_back(oneTrig);
						oneTriggerAtStation.triggers.back().trigIdx = j;
						triggerPool[trigSiteID] = oneTriggerAtStation;
					}
					else
					{
						//recycleIdx.push_back(j);
						triggerPool[trigSiteID].triggers.emplace_back(oneTrig);
					}
				}
			}
			//if ((triggerPool.size()) < 5)
			//{
			//	 for (int i = recycleIdx.size() - 1; i >= 0; i--) {
			//		allTriggers.erase(allTriggers.begin() + recycleIdx[i]);
			//	 }
			//	 continue;
			//}

			if ((triggerPool.size()) >= 5)
			{
				double MinSq = FLOAT_MAX;
				double ThresSqInitial = LocThresholdInitial;
				double ThresSqFinal = LocThresholdFinal;
				LocSta result;
				__int64 finalCombIdx;

				vector<vector<TriggerInfo>> locCombinationPool;
				//std::cout << "Before comb, Elapsed time: " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() << " seconds.\n";
				if (MinSq > ThresSqInitial)
				{
					locCombinationPool = LigTools::getLocationPool_p(triggerPool, siteTimeMap, triggerPool.size());
					if (locCombinationPool.size() > 0)
						ProcessCombinationPool(locCombinationPool, triggerPool, MinSq, result, finalCombIdx, CountGeoLocationTimes);
				}

				for (int i = 0; i < 2; ++i) //最大站点数为8，最少可信站点数为6，迭代2次
				{
					if (MinSq > ThresSqInitial && triggerPool.size() > 6)
					{
						if (locCombinationPool.size() > 0)
						{
							map<int, double> sqMap;
							for (auto& iter : locCombinationPool[finalCombIdx])
							{
								sqMap[iter.stationID] = abs( (iter.time.m_Sec + iter.time.m_ActPointSec - result.occur_t) * cVeo -
									Stadistance(siteMap[iter.stationID].latitude, siteMap[iter.stationID].longitude, result.Lat, result.Lon) );
							}
							// 找出sqMap中最大的值的编号，去除这个站点
							auto maxElement = max_element(sqMap.begin(), sqMap.end(), [](const pair<int, double>& p1, const pair<int, double>& p2) { return p1.second < p2.second; });
							triggerPool.erase(maxElement->first);
						}

						locCombinationPool = LigTools::getLocationPool_p(triggerPool, siteTimeMap, triggerPool.size());
						if (locCombinationPool.size() > 0)
							ProcessCombinationPool(locCombinationPool, triggerPool, MinSq, result, finalCombIdx, CountGeoLocationTimes);
					}
				}

				//if (MinSq > ThresSqInitial && triggerPool.size() > 6)
				//{
				//	locCombinationPool = LigTools::getLocationPool_p(triggerPool, siteTimeMap, triggerPool.size() - 1);
				//	if (locCombinationPool.size() > 0)
				//		ProcessCombinationPool(locCombinationPool, triggerPool, MinSq, result, finalCombIdx, CountGeoLocationTimes);
				//}

				//if (MinSq > ThresSqInitial && triggerPool.size() > 7)
				//{
				//	locCombinationPool = LigTools::getLocationPool_p(triggerPool, siteTimeMap, triggerPool.size() - 2);
				//	if (locCombinationPool.size() > 0)
				//		ProcessCombinationPool(locCombinationPool, triggerPool, MinSq, result, finalCombIdx, CountGeoLocationTimes);
				//}
				//std::cout << "After comb, Elapsed time: " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() << " seconds.\n";

				if (MinSq < ThresSqInitial)
				{
					vector<TriggerInfo>& oneComb = locCombinationPool[finalCombIdx];

					vector<double> Loc_Time_One;
					vector<LocSta> Stations_One;

					for (int j = 0; j < oneComb.size(); ++j)
					{
						Loc_Time_One.emplace_back(oneComb[j].time.m_Sec + oneComb[j].time.m_ActPointSec);
						Stations_One.emplace_back(triggerPool[oneComb[j].stationID].staLocation);
					}

					LocSta oneResult = result;
					// oneResult = FinalGeoLocation_GPU(Stations_One, Loc_Time_One, result);
					//oneResult = GeoLocation_OP(Stations_One, Loc_Time_One, result);
					oneResult = GeoLocation_OP_2(Stations_One, Loc_Time_One, result);

					LocSta oneResult_rad = oneResult;
					oneResult_rad.Lat = oneResult.Lat * degree2radians;
					oneResult_rad.Lon = oneResult.Lon * degree2radians;

					//ThresSqFinal = 1.5;
					//if (oneComb.size() >= 6) ThresSqFinal = 3;
					double distanceToBase = Stadistance(siteMap[oneComb[0].stationID].latitude, siteMap[oneComb[0].stationID].longitude, oneResult.Lat, oneResult.Lon);

					if (LigTools::check_location_structure(Stations_One, oneResult_rad, checkTheta) && oneResult.sq < ThresSqFinal && (distanceToBase < 1500.0))
					{
						cout << "CountGeoLocationTimes " << CountGeoLocationTimes << "  Number of sites: " << Stations_One.size() << endl;
						cout << CGPSTimeAlgorithm::GetTimeStr(oneComb[0].time) << " " << oneResult.Lat << " " << oneResult.Lon << " " << oneResult.h << " " << oneResult.sq << " " << distanceToBase << endl;
						// 把cout的内容写入NewData.txt里，调试使用
						// 改成覆盖写入模式
						GPSTime lig_time = oneComb[0].time;
						lig_time.set_second(oneResult.occur_t);
						outfile_O << CGPSTimeAlgorithm::GetTimeStr(lig_time) << " " << oneResult.Lat << " " << oneResult.Lon << " " << oneResult.h << " " << oneResult.sq << " "<< oneComb.size() << endl;
						postThreadPool.enqueue(LigDataApi::PostLigResult, lig_time, oneResult, oneComb, siteMap);
						CountLocationPoints++;
					}
					// cout << "Test2" << endl;
					// 需要删除的元素的索引
					std::vector<int> indices;
					for (int i = 0; i < oneComb.size(); ++i)
					{
						indices.emplace_back(oneComb[i].trigIdx);
						// cout << siteMap[oneComb[i].stationID].name << " " << oneComb[i].Value << " " << CGPSTimeAlgorithm::GetTimeStr(oneComb[i].time) << " " << Stadistance(siteMap[oneComb[i].stationID].latitude, siteMap[oneComb[i].stationID].longitude, oneResult.Lat, oneResult.Lon) << endl;
					}
					// 从后向前删除
					// shared_lock<shared_mutex> lock(rwMutex);
					std::sort(indices.begin(), indices.end(), std::greater<int>());
					for (auto i : indices)
					{
						allTriggers.erase(allTriggers.begin() + i);
					}
					continue;
				}
			}

			allTriggers.erase(allTriggers.begin());
			// 输出经过的时间
		}

		outfile_O.close();
		// 计算经过的时间（以秒为单位）
		std::cout << "CountLocationPoints: " << CountLocationPoints << " Elapsed time: " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() << " seconds.\n";

		if (config["mode"].as<string>() == "reProcess")
			break;
	}
}