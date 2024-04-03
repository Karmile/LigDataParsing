#include "WorkThreads.h"

void threadLoadData(deque<TriggerInfo>& transTriggers, LigDataApi& LigDataApi,
                    shared_mutex& rwMutex, YAML::Node& config, bool& keep_loading) {
  if (config["mode"].as<string>() == "reProcess") {
    GPSTime till_time(config["reProcess"]["startTime"].as<string>());
    GPSTime start_time(config["reProcess"]["startTime"].as<string>());
    GPSTime end_time(config["reProcess"]["endTime"].as<string>());
    while (1) {
      // ��ȡ���µ�����
      till_time += Duration("00000000T002000");
      vector<TriggerInfo> cache = LigDataApi.GetHistoricalTriggerDataUntill(till_time, 20);
      cache.erase(remove_if(cache.begin(), cache.end(),
                            [&](const TriggerInfo& trigger) {
                              return trigger.time < start_time || trigger.time > end_time;
                            }),
                  cache.end());

      // �ϲ�����
      int init_size = transTriggers.size();

      unique_lock<shared_mutex> lock(rwMutex);
      transTriggers.insert(transTriggers.end(), cache.begin(), cache.end());
      sort(transTriggers.begin(), transTriggers.end());  // ����ʱ������
      transTriggers.erase(unique(transTriggers.begin(), transTriggers.end()),
                          transTriggers.end());  // ȥ��
      lock.unlock();

      LOG_INFO("cache size " << cache.size() << endl);
      LOG_INFO("add " << transTriggers.size() - init_size << " new triggers" << endl);
      LOG_INFO("current transTriggers size: " << transTriggers.size() << endl);

      if (till_time >= end_time) {
        keep_loading = false;
        break;
      }
    }
  } else {
    while (1) {
      // ��ȡ���µ�����
      vector<TriggerInfo> cache = LigDataApi.GetRealTimeTriggerData();

      // �ϲ�����
      int init_size = transTriggers.size();

      unique_lock<shared_mutex> lock(rwMutex);
      transTriggers.insert(transTriggers.end(), cache.begin(), cache.end());
      sort(transTriggers.begin(), transTriggers.end());  // ����ʱ������
      transTriggers.erase(unique(transTriggers.begin(), transTriggers.end()),
                          transTriggers.end());  // ȥ��
      lock.unlock();

      LOG_INFO("cache size " << cache.size() << endl);
      LOG_INFO("add " << transTriggers.size() - init_size << " new triggers" << endl);
      LOG_INFO("current transTriggers size: " << transTriggers.size() << endl);

      std::this_thread::sleep_for(std::chrono::seconds(60));
    }
  }
}