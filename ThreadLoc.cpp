#include "WorkThreads.h"
#include <future>

void ThreadLoc(vector<TriggerInfo>& allTriggers, map<int, StationInfo>& siteMap, map<int, map<int, double>>& siteTimeMap, shared_mutex& rwMutex, YAML::Node config)
{
	static int current_size{ 1 };
	ThreadPool postThreadPool{ 100 };
	// 用于测试
	while (1) {
		if (current_size == allTriggers.size())
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}
		// 开始定位
		shared_lock<shared_mutex> lock(rwMutex);
		auto start = std::chrono::high_resolution_clock::now();

		ofstream outfile_O;
		outfile_O.open("lig_txt/NewData2.txt", ios::out);

		while (allTriggers.size())
		{
			map<int, triggerAtStation> triggerPool;

			TriggerInfo& baseTrig = allTriggers[0];
			vector<int> recycleIdx;
			// recycleIdx.push_back(curLoopIdx);

			// Wait time
			// if((allTriggers.back().time - baseTrig.time) < config["waitTime"].as<double>()) continue;

			for (int j = 0; j < allTriggers.size(); ++j)
			{
				TriggerInfo& oneTrig = allTriggers[j];
				int trigSiteID = oneTrig.stationID;
				double diffTime = baseTrig.time - oneTrig.time;

				if ((diffTime < -(0.003)) || (diffTime > 0))
					break;

				// std::cout << diffTime << endl;

				// 这里第一个基站只放入一个数据，因为筛选标准是以第一个基站定标的，所以第一个基站只放入一个数据
				if ((fabs(diffTime) <= siteTimeMap[baseTrig.stationID][oneTrig.stationID])) // || (baseTrig.stationID == oneTrig.stationID))
				{
					recycleIdx.push_back(j);

					// 判断键值是否存在
					if (triggerPool.find(trigSiteID) == triggerPool.end())
					{
						triggerAtStation oneTriggerAtStation;
						oneTriggerAtStation.staLocation = (LocSta(siteMap[trigSiteID].latitude * degree2radians,
							siteMap[trigSiteID].longitude * degree2radians,
							siteMap[trigSiteID].altitude / 1000.0));
						oneTriggerAtStation.stationID = trigSiteID;
						oneTriggerAtStation.triggers.push_back(oneTrig);
						oneTriggerAtStation.triggers.back().trigIdx = j;
						triggerPool[trigSiteID] = oneTriggerAtStation;
					}
					else
					{
						triggerPool[trigSiteID].triggers.push_back(oneTrig);
					}
				}
			}

			if ((triggerPool.size()) >= 5)
			{
				vector<vector<TriggerInfo>> locCombinationPool = LigTools::getLocationPool_p(triggerPool, siteTimeMap, triggerPool.size());

				if (triggerPool.size() > 6) {
					for (int i = 1; i <= 2; ++i) {
						vector<vector<TriggerInfo>> locCombinationPool_n = LigTools::getLocationPool_p(triggerPool, siteTimeMap, triggerPool.size() - i);
						locCombinationPool_n.erase(std::remove_if(locCombinationPool_n.begin(), locCombinationPool_n.end(), [&](const auto& combination) {
							return std::none_of(combination.begin(), combination.end(), [&](const auto& trigger) {
								return trigger.stationID == baseTrig.stationID;
							});
						}), locCombinationPool_n.end());
						locCombinationPool.insert(locCombinationPool.end(), locCombinationPool_n.begin(), locCombinationPool_n.end());
						
						if(triggerPool.size() ==7) break;
					}
				}

				if (locCombinationPool.size() == 0)
				{
					allTriggers.erase(allTriggers.begin());
					continue;
				}

				double MinSq = FLOAT_MAX;
				LocSta result;
				__int64 finalCombIdx = 0;

				double* sqList;
				sqList = new double[locCombinationPool.size()];
				LocSta* resultList = new LocSta[locCombinationPool.size()];

#pragma omp parallel for
				for (int m = 0; m < locCombinationPool.size(); ++m)
				{
					vector<TriggerInfo>& oneComb = locCombinationPool[m];
		/*			if (oneComb[0].stationID != baseTrig.stationID) {
						sqList[m] = FLOAT_MAX;
						continue;
					}*/
					if(oneComb.size() < 5){
						sqList[m] = FLOAT_MAX;
						continue;
					}
					vector<double> Loc_Time_One;
					vector<LocSta> Stations_One;

					for (int j = 0; j < oneComb.size(); ++j)
					{
						Loc_Time_One.push_back(oneComb[j].time.m_Sec + oneComb[j].time.m_ActPointSec);
						Stations_One.push_back(triggerPool[oneComb[j].stationID].staLocation);
					}

					LocSta oneResult = GeoLocation_GPU(Stations_One, Loc_Time_One);
					//LocSta oneResult = GeoLocation_OP(Stations_One, Loc_Time_One);

					sqList[m] = oneResult.sq;
					resultList[m] = oneResult;
				}
				// 查找最小值
				CountGeoLocationTimes += locCombinationPool.size();
				MinSq = *min_element(sqList, sqList + locCombinationPool.size());
				finalCombIdx = min_element(sqList, sqList + locCombinationPool.size()) - sqList;
				result = resultList[finalCombIdx];

				delete[] sqList;
				delete[] resultList;

				if (MinSq < 30.0)
				{
					vector<TriggerInfo>& oneComb = locCombinationPool[finalCombIdx];

					vector<double> Loc_Time_One;
					vector<LocSta> Stations_One;

					for (int j = 0; j < oneComb.size(); ++j)
					{
						Loc_Time_One.push_back(oneComb[j].time.m_Sec + oneComb[j].time.m_ActPointSec);
						Stations_One.push_back(triggerPool[oneComb[j].stationID].staLocation);
					}

					LocSta oneResult = result;
					// oneResult = FinalGeoLocation_GPU(Stations_One, Loc_Time_One, result);
					oneResult = GeoLocation_OP(Stations_One, Loc_Time_One, result);

					CountLocationPoints++;
					LocSta oneResult_rad = oneResult;
					oneResult_rad.Lat = oneResult.Lat * degree2radians;
					oneResult_rad.Lon = oneResult.Lon * degree2radians;
					if (LigTools::check_location_structure(Stations_One, oneResult_rad, config["checkTheta"].as<double>()))
					{
						cout << "CountGeoLocationTimes " << CountGeoLocationTimes << endl;
						cout << CGPSTimeAlgorithm::GetTimeStr(oneComb[0].time) << " " << oneResult.Lat << " " << oneResult.Lon << " " << oneResult.h << " " << oneResult.sq << endl;
						// 把cout的内容写入NewData.txt里，调试使用
						// 改成覆盖写入模式
						GPSTime lig_time = oneComb[0].time;
						lig_time.set_second(oneResult.occur_t);
						outfile_O << CGPSTimeAlgorithm::GetTimeStr(lig_time) << " " << oneResult.Lat << " " << oneResult.Lon << " " << oneResult.h << " " << oneResult.sq << endl;
						//LigDataApi::PostLigResult(lig_time, oneResult, oneComb, siteMap);
						postThreadPool.enqueue(LigDataApi::PostLigResult, lig_time, oneResult, oneComb, siteMap);
					}
					// cout << "Test2" << endl;
					// 需要删除的元素的索引
					std::vector<int> indices;
					for (int i = 0; i < oneComb.size(); ++i)
					{
						indices.push_back(oneComb[i].trigIdx);
						//cout << siteMap[oneComb[i].stationID].name << " " << oneComb[i].Value << " " << CGPSTimeAlgorithm::GetTimeStr(oneComb[i].time) << " " << Stadistance(siteMap[oneComb[i].stationID].latitude, siteMap[oneComb[i].stationID].longitude, oneResult.Lat, oneResult.Lon) << endl;
					}
					// 从后向前删除
				    //shared_lock<shared_mutex> lock(rwMutex);
					std::sort(indices.begin(), indices.end(), std::greater<int>());
					for (auto i : indices)
					{
						allTriggers.erase(allTriggers.begin() + i);
					}
					continue;
				}
			}

			// shared_lock<shared_mutex> lock(rwMutex);
			// for (int i = recycleIdx.size() - 1; i >= 0; i--) {
			//	allTriggers.erase(allTriggers.begin() + recycleIdx[i]);
			// }
			// lock.unlock();
			//	allTriggers.front().releaseData();
			allTriggers.erase(allTriggers.begin());

			// 输出经过的时间
		}
		lock.unlock();
		outfile_O.close();
		// 获取当前时间点
		auto end = std::chrono::high_resolution_clock::now();
		// 计算经过的时间（以秒为单位）
		double elapsed_seconds = std::chrono::duration<double>(end - start).count();
		std::cout << "Elapsed time: " << elapsed_seconds << " seconds.\n";
		current_size = allTriggers.size();
		if (config["mode"].as<string>() == "reProcess")
		{
			break;
		}
	}
}