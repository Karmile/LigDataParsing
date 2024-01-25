#include "parse_json.h"
#include "iostream"
#include "GPSTimeAlgorithm.h"
#include "LocStruct.h"
#include <thread>
#include <map>
#include "Params.h"
#include "GeoTools.h"
#include <shared_mutex>

using namespace std;
shared_mutex rwMutex;
std::string str = (".\\config.yaml");
void threadLoadData(vector<TriggerInfo>& allTriggers, Parser& parser){
    
    while (1){
        // 获取最新的数据
        {
            unique_lock<shared_mutex> lock(rwMutex);
            vector<TriggerInfo> cache = parser.GetTriggersData();
            // 合并数据
            int init_size = allTriggers.size();
            allTriggers.insert(allTriggers.end(), cache.begin(), cache.end());
            sort(allTriggers.begin(), allTriggers.end()); // 按照时间排序
            allTriggers.erase(unique(allTriggers.begin(), allTriggers.end()), allTriggers.end()); // 去重
            cout << "add " << allTriggers.size() - init_size << " new triggers" << endl;
            cout <<"current alltriggers size: " << allTriggers.size() << endl;
            lock.unlock();
        }

        break;
        // 休眠30s
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void threadShrinkData(vector<TriggerInfo>& allTriggers) {

    YAML::Node config = YAML::LoadFile(str);
    while (1) {
        if (allTriggers.size())
        {
            {
                unique_lock<shared_mutex> lock(rwMutex);
                //The proportion of interval to sampling time
                //scale与采样时间、此函数休眠时间相关,scale = T/t2
                int n = allTriggers.size() / config["scale"].as<int>();
                allTriggers.erase(allTriggers.begin(), allTriggers.begin() + n);
                cout << "removed " << n << " old triggers" << endl << endl;
                lock.unlock();
            }
            // 休眠30s 
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

    }
}


vector<vector<TriggerInfo>> getLocationPool(map<int, triggerAtStation>& triggerPool)
{

    vector<vector<TriggerInfo>> locCombinationPool;
    int m = 0;
    for (auto & oneStation: triggerPool)
    {
        int nPool = locCombinationPool.size();
        int cachePoolLimit = oneStation.second.triggers.size();

        if (m)
        {
            if (nPool == 0) break;
            for (int k = cachePoolLimit - 1; k > -1; --k)
            {
                if (k)
                {
                    for (int p = 0; p < nPool; ++p)
                    {
                        locCombinationPool.push_back(locCombinationPool[p]);
                        locCombinationPool.back().push_back(oneStation.second.triggers[k]);
                    }
                }
                else
                {
                    for (int p = nPool - 1; p > -1; --p)
                    {
                        locCombinationPool[p].push_back(oneStation.second.triggers[k]);
                    }
                }
            }
        }
        else
        {
            for (int k = cachePoolLimit - 1; k > -1; --k)
            {
                vector<TriggerInfo> oneComb;
                oneComb.push_back(oneStation.second.triggers[k]);
                locCombinationPool.push_back(oneComb);
            }
        }
        m++;
    }

    return locCombinationPool;
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

    // 新线程，连续获取数据
    thread loader([&]() {
        threadLoadData(allTriggers, parser);
        });


    //// 新线程，压缩数据
    //thread shrinker([&]() {
    //    threadShrinkData(allTriggers);
    //    });

    ////test
    //while (1)
    //{  
    //    if (allTriggers.size())
    //    {
    //        shared_lock<shared_mutex> lock(rwMutex);
    //        for (auto &trigger : allTriggers)    
    //        {
    //            //cout << trigger.stationID << endl;
    //            trigger.stationID = 111;
    //        }
    //        cout << allTriggers.size() << endl;
    //    }
    //}

    // while(1){
    //     int nCount = allTriggers.size();
    //     cout << "allTriggers have " << nCount << " points." <<endl;
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
    // }


    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 开始定位
	//GPSTime endTime = allTriggers.back().time;
    while (1)
    {
        while (allTriggers.size())
        {   
            shared_lock<shared_mutex> lock(rwMutex);
            int idx = 0;
            for (const auto &x : allTriggers) {
                cout << CGPSTimeAlgorithm::GetTimeStr(x.time) << " " << x.stationID << endl;
                idx++;
                if (idx == 300) break;
            }

            int curLoopIdx = 0;
            // vector<triggerAtStation> triggerPool;
            map<int, triggerAtStation> triggerPool;
            //GPSTime baseTime = allTriggers[curLoopIdx].time;
            TriggerInfo& baseTrig = allTriggers[curLoopIdx];
            vector<int> recycleIdx;
            //recycleIdx.push_back(curLoopIdx);

            // Wait time
            //if((allTriggers.back().time - baseTrig.time) < config["waitTime"].as<double>()) continue;

            for (int j = curLoopIdx; j < allTriggers.size(); ++j)
            {
                TriggerInfo& oneTrig = allTriggers[j];
                int trigSiteID = oneTrig.stationID;
                double diffTime = baseTrig.time - oneTrig.time;

                if ((diffTime < -(0.003)) || (diffTime > 0)) continue;

                //std::cout << diffTime << endl;

                if ((fabs(diffTime) < siteTimeMap[baseTrig.stationID][oneTrig.stationID])
                    ||(baseTrig.stationID == oneTrig.stationID))
                {
                    recycleIdx.push_back(j);

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

            // curLoopIdx++;

            if ((triggerPool.size()) >= 5)
            {
                vector<vector<TriggerInfo>> locCombinationPool = getLocationPool(triggerPool);

                double MinSq = 10000000000000;
                LocSta result;
                int finalCombIdx = 0;

#pragma omp parallel for
                for (int m = 0; m < locCombinationPool.size(); ++m)
                {
                    vector<TriggerInfo>& oneComb = locCombinationPool[m];

                    vector<double> Loc_Time_One;
                    vector<LocSta> Stations_One;

                    for (int j = 0; j < oneComb.size(); ++j)
                    {
                        Loc_Time_One.push_back(oneComb[j].time.m_Sec + oneComb[j].time.m_ActPointSec);
                        Stations_One.push_back(triggerPool[oneComb[j].stationID].staLocation);
                    }

                    LocSta oneResult = GeoLocation(Stations_One, Loc_Time_One);
                    //LocSta oneResult1 = GeoLocation_OP(Stations_One, Loc_Time_One);
                    CountGeoLocationTimes++;

                    if (oneResult.sq < MinSq)
                    {
                        MinSq = oneResult.sq;
                        result = oneResult;
                        finalCombIdx = m;
                    }
                }

                if (MinSq < 10.0)
                {
                    vector<TriggerInfo>& oneComb = locCombinationPool[finalCombIdx];

                    vector<double> Loc_Time_One;
                    vector<LocSta> Stations_One;

                    for (int j = 0; j < oneComb.size(); ++j)
                    {
                        Loc_Time_One.push_back(oneComb[j].time.m_Sec + oneComb[j].time.m_ActPointSec);
                        Stations_One.push_back(triggerPool[oneComb[j].stationID].staLocation);
                    }

                    LocSta oneResult = FinalGeoLocation(Stations_One, Loc_Time_One, result);
                    cout << CGPSTimeAlgorithm::GetTimeStr(oneComb[0].time) << " " << oneResult.Lat << " " << oneResult.Lon << " " << oneResult.h << " " << oneResult.sq << endl;

                    int cc = 1;

                    ////////////////////
                    ////////////////////

                    //for (int j = 0; j < oneComb.size(); ++j)
                    //{
                    //	allTriggers[oneComb[j].m_idx].releaseData();
                    //	allTriggers.erase(allTriggers.begin() + oneComb[j].m_idx);
                    //}

                    /*continue*/;
                }
            }


            for (int i = recycleIdx.size() - 1; i >= 0; i--) {
                allTriggers.erase(allTriggers.begin() + recycleIdx[i]);
            }
            //	allTriggers.front().releaseData();
            //	allTriggers.erase(allTriggers.begin());
        }
    }
    loader.join();
    return 0;
}