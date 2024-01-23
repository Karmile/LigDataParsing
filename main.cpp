#include "parse_json.h"
#include "iostream"
#include "GPSTimeAlgorithm.h"
#include "LocStruct.h"
#include <thread>
#include <map>
#include "Params.h"
#include "GeoTools.h"

using namespace std;

void threadLoadData(vector<TriggerInfo>& allTriggers, Parser& parser){
    while (1){
        // 获取最新的数据
        vector<TriggerInfo> cache = parser.GetTriggersData();

        // 合并数据 ---需要添加线程锁
        allTriggers.insert(allTriggers.end(), cache.begin(), cache.end());
        sort(allTriggers.begin(), allTriggers.end()); // 按照时间排序
        allTriggers.erase(unique(allTriggers.begin(), allTriggers.end()), allTriggers.end()); // 去重
        // ------------------

        break;
        // 休眠30s
        std::this_thread::sleep_for(std::chrono::seconds(30));

    }
}


int main() {
    std::string configFile = (".\\config.yaml");
	YAML::Node config;

    try {
        config = YAML::LoadFile(configFile);
        std::cout << "load config successfully!\n"  << endl;
    }
    catch (const YAML::Exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    Parser parser(configFile);
    
    // sites 不用重复获取，一段时间内获取一次即可
    vector<StationInfo> sites = parser.GetStationData();

    map<int, StationInfo> siteMap;
    for (auto& site : sites){
        siteMap[site.stationID] = site;
    }
    map<int, map<int, double>> siteDistMap, siteTimeMap;
    for (auto& site1 : sites){
        for (auto& site2 : sites){
            siteDistMap[site1.stationID][site2.stationID] = Stadistance(site1.latitude, site1.longitude, site2.latitude, site2.longitude);
            siteTimeMap[site1.stationID][site2.stationID] = siteDistMap[site1.stationID][site2.stationID] / cVeo;
        }
    }

    // allTriggers 要求每30s刷新，
    vector<TriggerInfo> allTriggers;

    // 新进程，连续获取数据
    thread loader([&]() {
        threadLoadData(allTriggers, parser);
        });

    // while(1){
    //     int nCount = allTriggers.size();
    //     cout << "allTriggers have " << nCount << " points." <<endl;
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
    // }

    int curLoopIdx = 0;

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 开始定位
	//GPSTime endTime = allTriggers.back().time;

    //
	while (allTriggers.size())
	{
		// vector<triggerAtStation> triggerPool;
        map<int, triggerAtStation> triggerPool;
        //GPSTime baseTime = allTriggers[curLoopIdx].time;
        TriggerInfo& baseTrig = allTriggers[curLoopIdx];

        // Wait time
        if((allTriggers.back().time - baseTrig.time) < config["waitTime"].as<double>()) continue;

		for (int j = curLoopIdx+1; j < allTriggers.size() - 1; ++j)
		{
			TriggerInfo& oneTrig = allTriggers[j];
            int trigSiteID = oneTrig.stationID;
			double diffTime = oneTrig.time - baseTrig.time;

			if ((diffTime < -(0.003)) || (diffTime > 0)) break;

            std::cout << diffTime << endl;
			
            if (fabs(diffTime) < siteTimeMap[trigSiteID][allTriggers[j].stationID])
            {
                //判断键值是否存在
                if (triggerPool.find(trigSiteID) == triggerPool.end())
                {
                    triggerAtStation oneTriggerAtStation;
                    oneTriggerAtStation.staLocation = (LocSta(siteMap[trigSiteID].latitude * degree2radians,
                                                            siteMap[trigSiteID].longitude * degree2radians,
                                                            siteMap[trigSiteID].altitude / 1000.0));
                    oneTriggerAtStation.stationID = trigSiteID;
                    oneTriggerAtStation.triggers.push_back(oneTrig);
                    triggerPool[trigSiteID] = oneTriggerAtStation;
                }
                else
                {
                    triggerPool[trigSiteID].triggers.push_back(oneTrig);
                }
            }
		}

        int cc = 1;
        curLoopIdx++;
// 		if ((locCachePool.size()) >= 5)
// 		{
// 			vector<vector<triggerElement>> locCombinationPool = getLocationPool(locCachePool);

// 			double MinSq = 10000000000000;
// 			LocSta result;
// 			int finalCombIdx = 0;

// #pragma omp parallel for
// 			for (int m = 0; m < locCombinationPool.size(); ++m)
// 			{
// 				vector<triggerElement>& oneComb = locCombinationPool[m];

// 				vector<double> Loc_Time_One;
// 				vector<LocSta> Stations_One;

// 				for (int j = 0; j < oneComb.size(); ++j)
// 				{
// 					Loc_Time_One.push_back(oneComb[j].m_time);
// 					Stations_One.push_back(locCachePool[j].staLocation);
// 				}

// 				LocSta oneResult = GeoLocation(Stations_One, Loc_Time_One);

// 				CountGeoLocationTimes++;

// 				if (oneResult.sq < MinSq)
// 				{
// 					MinSq = oneResult.sq;
// 					result = oneResult;
// 					finalCombIdx = m;
// 				}
// 			}

// 			if (MinSq < 10.0)
// 			{
// 				vector<triggerElement>& oneComb = locCombinationPool[finalCombIdx];

// 				vector<double> Loc_Time_One;
// 				vector<LocSta> Stations_One;

// 				for (int j = 0; j < oneComb.size(); ++j)
// 				{
// 					Loc_Time_One.push_back(oneComb[j].m_time);
// 					Stations_One.push_back(locCachePool[j].staLocation);
// 				}

// 				LocSta oneResult = FinalGeoLocation(Stations_One, Loc_Time_One, result);

// 				////////////////////
// 				////////////////////

// 				for (int j = 0; j < oneComb.size(); ++j)
// 				{
// 					allTriggers[oneComb[j].m_idx].releaseData();
// 					allTriggers.erase(allTriggers.begin() + oneComb[j].m_idx);
// 				}

// 				continue;
// 			}
		// }

		//allTriggers.front().releaseData();
		//allTriggers.erase(allTriggers.begin());
	}
    loader.join();
    return 0;
}