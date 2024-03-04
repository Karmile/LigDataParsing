#include "LigDataApi.h"
#include "GPSTimeAlgorithm.h"
#include "Params.h"
#include <thread>
#include <map>
#include <shared_mutex>
#include <chrono>
#include "WorkThreads.h"
#include <unordered_map>

using namespace std;
shared_mutex rwMutex;
std::string str = (".\\config.yaml");

int main()
{
	std::string configFile = (".\\config.yaml");
	YAML::Node config;

	try
	{
		config = YAML::LoadFile(configFile);
		std::cout << "load config successfully!\n"
				  << endl;
	}
	catch (const YAML::Exception &e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	LigDataApi LigDataApi(configFile);

	// sites 不用重复获取，一段时间内获取一次即可
	vector<StationInfo> sites = LigDataApi.GetStationData();

	unordered_map<int, StationInfo> siteMap;
	for (auto &site : sites)
	{
		siteMap[site.stationID] = site;
	}
	
	unordered_map<int, unordered_map<int, double>> siteDistMap, siteTimeMap;

	for (auto &site1 : sites)
	{
		for (auto &site2 : sites)
		{
			siteDistMap[site1.stationID][site2.stationID] = Stadistance(site1.latitude, site1.longitude, site2.latitude, site2.longitude);
			siteTimeMap[site1.stationID][site2.stationID] = siteDistMap[site1.stationID][site2.stationID] / cVeo;
		}
	}

	thread loader, locThread;
	// allTriggers 要求每30s刷新，
	deque<TriggerInfo> allTriggers;
	deque<TriggerInfo> transTriggers;
	bool keep_loading{ true };

	// 开始定位
	locThread = thread([&]()
		{ ThreadLoc(allTriggers, transTriggers, siteMap, siteTimeMap, rwMutex, config, keep_loading); });
	if (config["mode"].as<string>() == "reProcess")
	{
		loader = thread([&]()
			{ threadLoadData(transTriggers, LigDataApi, rwMutex, keep_loading, true); });
	}
	else if (config["mode"].as<string>() == "realTime")
	{
		// 新线程，实时获取数据
		loader = thread([&]()
			{ threadLoadData(transTriggers, LigDataApi, rwMutex, keep_loading, false); });
	}


	locThread.join();
	loader.join();
	return 0;
}