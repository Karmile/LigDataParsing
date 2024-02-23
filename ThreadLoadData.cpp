#include "WorkThreads.h"

void threadLoadData(deque<TriggerInfo>& transTriggers, LigDataApi& LigDataApi, shared_mutex& rwMutex, bool isReProcess) {

	while (1) {
		// 获取最新的数据
		{
			vector<TriggerInfo> cache = LigDataApi.GetTriggersData();
			// 合并数据
			int init_size = transTriggers.size();

			unique_lock<shared_mutex> lock(rwMutex);
			transTriggers.insert(transTriggers.end(), cache.begin(), cache.end());
			sort(transTriggers.begin(), transTriggers.end()); // 按照时间排序
			transTriggers.erase(unique(transTriggers.begin(), transTriggers.end()), transTriggers.end()); // 去重

			lock.unlock();

			//if (transTriggers.size())
			//{
			//	for (int i = 1; i != transTriggers.size(); ++i) {
			//		if (transTriggers[i].time == transTriggers[i - 1].time) {
			//			cout << "Debug: " << transTriggers[i - 1].stationID << " " << transTriggers[i].stationID << endl;
			//			cout << CGPSTimeAlgorithm::GetTimeStr(transTriggers[i - 1].time) << endl;
			//			cout << CGPSTimeAlgorithm::GetTimeStr(transTriggers[i].time) << endl;
			//		}
			//	}
			//}


			cout << "cache size " << cache.size() << endl;
			cout << "add " << transTriggers.size() - init_size << " new triggers" << endl;
			cout << "current transTriggers size: " << transTriggers.size() << endl;

			if (isReProcess) break;
		}

		//break;
		// 休眠30s
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}