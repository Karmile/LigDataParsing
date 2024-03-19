#include "WorkThreads.h"

void threadLoadData(deque<TriggerInfo>& transTriggers, LigDataApi& LigDataApi, shared_mutex& rwMutex, YAML::Node& config, bool isReProcess) {



	if (isReProcess)
	{
		GPSTime till_time(config["reProcess"]["startTime"].as<string>());
		GPSTime start_time(config["reProcess"]["startTime"].as<string>());
		GPSTime end_time(config["reProcess"]["endTime"].as<string>());
		while (1) {

			// 获取最新的数据
			till_time += Duration("00000000T002000");
			vector<TriggerInfo> cache = LigDataApi.GetHistoricalTriggerDataUntill(till_time, 20);
			cache.erase(remove_if(cache.begin(), cache.end(), [&](const TriggerInfo& trigger) {
				return trigger.time < start_time || trigger.time > end_time;
				}), cache.end());


			// 合并数据
			int init_size = transTriggers.size();

			unique_lock<shared_mutex> lock(rwMutex);
			transTriggers.insert(transTriggers.end(), cache.begin(), cache.end());
			sort(transTriggers.begin(), transTriggers.end()); // 按照时间排序
			transTriggers.erase(unique(transTriggers.begin(), transTriggers.end()), transTriggers.end()); // 去重
			lock.unlock();


			cout << "cache size " << cache.size() << endl;
			cout << "add " << transTriggers.size() - init_size << " new triggers" << endl;
			cout << "current transTriggers size: " << transTriggers.size() << endl;

			if (till_time >= end_time)
				break;
		}
	}
	else
	{
		while (1) {

			// 获取最新的数据
			vector<TriggerInfo> cache = LigDataApi.GetRealTimeTriggerData();

			// 合并数据
			int init_size = transTriggers.size();

			unique_lock<shared_mutex> lock(rwMutex);
			transTriggers.insert(transTriggers.end(), cache.begin(), cache.end());
			sort(transTriggers.begin(), transTriggers.end()); // 按照时间排序
			transTriggers.erase(unique(transTriggers.begin(), transTriggers.end()), transTriggers.end()); // 去重
			lock.unlock();


			cout << "cache size " << cache.size() << endl;
			cout << "add " << transTriggers.size() - init_size << " new triggers" << endl;
			cout << "current transTriggers size: " << transTriggers.size() << endl;

			std::this_thread::sleep_for(std::chrono::seconds(60));

		}

	}

}