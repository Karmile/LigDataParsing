#include "WorkThreads.h"

void threadLoadData(vector<TriggerInfo>& allTriggers, LigDataApi& LigDataApi, shared_mutex& rwMutex, bool isReProcess) {

	while (1) {
		// ��ȡ���µ�����
		{
			vector<TriggerInfo> cache = LigDataApi.GetTriggersData();
			// �ϲ�����
			int init_size = allTriggers.size();

			unique_lock<shared_mutex> lock(rwMutex);
			allTriggers.insert(allTriggers.end(), cache.begin(), cache.end());
			sort(allTriggers.begin(), allTriggers.end()); // ����ʱ������
			allTriggers.erase(unique(allTriggers.begin(), allTriggers.end()), allTriggers.end()); // ȥ��

			for (int i = 1; i != allTriggers.size(); ++i) {
				if (allTriggers[i].time == allTriggers[i - 1].time) {
					cout << "Debug: " << allTriggers[i - 1].stationID << " " << allTriggers[i].stationID << endl;
					cout << CGPSTimeAlgorithm::GetTimeStr(allTriggers[i - 1].time) << endl;
					cout << CGPSTimeAlgorithm::GetTimeStr(allTriggers[i].time) << endl;
				}
			}

			lock.unlock();

			cout << "cache size " << cache.size() << endl;
			cout << "add " << allTriggers.size() - init_size << " new triggers" << endl;
			cout << "current alltriggers size: " << allTriggers.size() << endl;

			if (isReProcess) break;
		}

		//break;
		// ����30s
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}