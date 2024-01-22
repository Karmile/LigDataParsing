#include "parse_json.h"
#include "iostream"
#include "GPSTimeAlgorithm.h"
#include "LocStruct.h"
#include <thread>
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



int main() {
    // ����HTTPS�����ȡվ����Ϣ

    Parser parser(str);

    // sites 不用重复获取，一段时间内获取一次即可
    vector<StationInfo> sites = parser.GetStationData();

    // allTriggers 要求每30s刷新，
    vector<TriggerInfo> allTriggers;

    // 新线程，连续获取数据
    thread loader([&]() {
        threadLoadData(allTriggers, parser);
        });
    // 新线程，压缩数据
    thread shrinker([&]() {
        threadShrinkData(allTriggers);
        });

    //test
    while (1)
    {  
        if (allTriggers.size())
        {
            shared_lock<shared_mutex> lock(rwMutex);
            for (auto &trigger : allTriggers)    
            {
                //cout << trigger.stationID << endl;
                trigger.stationID = 111;
            }
            cout << allTriggers.size() << endl;
        }
    }


    int nCount = allTriggers.size();
    int curLoopIdx = 0;
    
	//double endTime = CGPSTimeAlgorithm::GetGPSTimeFromHour(allTriggers.back().m_time);
//
//	while (allTriggers.size())
//	{
//		//����ö�����ж�λ��ϵ�Ԥ��������
//		vector<triggerCachePool> locCachePool;
//
//		double baseTime = CGPSTimeAlgorithm::GetGPSTimeFromHour(allTriggers[curLoopIdx].m_time);
//
//		if ((baseTime > endTime) | (endTime - baseTime) < 0.00025)
//		{
//			return 1;
//		}
//
//		unsigned long curjEndIdx = 0;
//		for (int j = curLoopIdx; j < allTriggers.size() - 1; ++j)
//		{
//			auto& oneSignal = allTriggers[j];
//			double trigTime = CGPSTimeAlgorithm::GetGPSTimeFromHour(oneSignal.m_time);
//			double diffTime = baseTime - trigTime; //����վ����ʱ���
//
//			if ((diffTime < -(0.00025)) | (diffTime > 0))
//			{
//				//curjEndIdx = j;
//				break;
//			}
//			else // if (fabs(Dif_Time) < timeDiff + 0.00001)
//			{
//				bool isNewSta = true;
//
//				for (auto& elem : locCachePool)
//				{
//					if (elem.staID == oneSignal.iSiteID)
//					{
//						elem.triggerPool.emplace_back(triggerElement(trigTime, j, oneSignal.m_vpp));
//						isNewSta = false;
//					}
//				}
//
//				if (isNewSta)
//				{
//					triggerCachePool onePool;
//					onePool.staLocation = (LocSta(oneSignal.lat * PI / 180, oneSignal.lon * PI / 180, oneSignal.alt / 1000.0)); //���ĳ��վ�������������Ĵ����¼� ��flag��1��Ϊ���
//					onePool.staID = oneSignal.iSiteID;
//					//trigCacPool.staName = stations[m].staName;
//					onePool.triggerPool.emplace_back(triggerElement(trigTime, j, oneSignal.m_vpp));
//					locCachePool.push_back(onePool);
//				}
//			}
//		}
//
//		if ((locCachePool.size()) >= 5)
//		{
//			vector<vector<triggerElement>> locCombinationPool = getLocationPool(locCachePool);
//
//			double MinSq = 10000000000000;
//			LocSta result;
//			int finalCombIdx = 0;
//
//#pragma omp parallel for
//			for (int m = 0; m < locCombinationPool.size(); ++m)
//			{
//				vector<triggerElement>& oneComb = locCombinationPool[m];
//
//				vector<double> Loc_Time_One;
//				vector<LocSta> Stations_One;
//
//				for (int j = 0; j < oneComb.size(); ++j)
//				{
//					Loc_Time_One.push_back(oneComb[j].m_time);
//					Stations_One.push_back(locCachePool[j].staLocation);
//				}
//
//				LocSta oneResult = GeoLocation(Stations_One, Loc_Time_One);
//
//				CountGeoLocationTimes++;
//
//				if (oneResult.sq < MinSq)
//				{
//					MinSq = oneResult.sq;
//					result = oneResult;
//					finalCombIdx = m;
//				}
//			}
//
//			if (MinSq < 10.0)
//			{
//				vector<triggerElement>& oneComb = locCombinationPool[finalCombIdx];
//
//				vector<double> Loc_Time_One;
//				vector<LocSta> Stations_One;
//
//				for (int j = 0; j < oneComb.size(); ++j)
//				{
//					Loc_Time_One.push_back(oneComb[j].m_time);
//					Stations_One.push_back(locCachePool[j].staLocation);
//				}
//
//				LocSta oneResult = FinalGeoLocation(Stations_One, Loc_Time_One, result);
//
//				////////////////////
//				////////////////////
//
//				for (int j = 0; j < oneComb.size(); ++j)
//				{
//					allTriggers[oneComb[j].m_idx].releaseData();
//					allTriggers.erase(allTriggers.begin() + oneComb[j].m_idx);
//				}
//
//				continue;
//			}
//		}
//
//		allTriggers.front().releaseData();
//		allTriggers.erase(allTriggers.begin());
//	}
    loader.join();
    return 0;
}