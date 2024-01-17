#include "parse_json.h"
#include "iostream"
#include "GPSTimeAlgorithm.h"
#include "LocStruct.h"

using namespace std;

int main() {
    // 发起HTTPS请求获取站点信息
    std::string str = (".\\config.yaml");
    Parser parser(str);

    vector<StationInfo> sites;
    vector<TriggerInfo> allTriggers;

    parser.parse_station_json(sites);
    parser.parse_trigger_json(allTriggers);


	int nCount = allTriggers.size();
	int curLoopIdx = 0;

	//double endTime = CGPSTimeAlgorithm::GetGPSTimeFromHour(allTriggers.back().m_time);
//
//	while (allTriggers.size())
//	{
//		//用于枚举所有定位组合的预处理向量
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
//			double diffTime = baseTime - trigTime; //计算站点间的时间差
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
//					onePool.staLocation = (LocSta(oneSignal.lat * PI / 180, oneSignal.lon * PI / 180, oneSignal.alt / 1000.0)); //如果某个站点有满足条件的触发事件 则flag加1作为标记
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

    return 0;
}