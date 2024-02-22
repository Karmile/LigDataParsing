#include "LigDataApi.h"
#include "GPSTimeAlgorithm.h"
#include "Params.h"
#include <thread>
#include <map>
#include <shared_mutex>
#include <chrono>
#include "WorkThreads.h"

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

	map<int, StationInfo> siteMap;
	for (auto &site : sites)
	{
		siteMap[site.stationID] = site;
	}
	
	map<int, map<int, double>> siteDistMap, siteTimeMap;

	for (auto &site1 : sites)
	{
		for (auto &site2 : sites)
		{
			siteDistMap[site1.stationID][site2.stationID] = Stadistance(site1.latitude, site1.longitude, site2.latitude, site2.longitude);
			siteTimeMap[site1.stationID][site2.stationID] = siteDistMap[site1.stationID][site2.stationID] / cVeo;
		}
	}

	thread loader;
	// allTriggers 要求每30s刷新，
	vector<TriggerInfo> allTriggers;
	if (config["mode"].as<string>() == "reProcess")
	{
		threadLoadData(allTriggers, LigDataApi, rwMutex, true);
	}
	else if (config["mode"].as<string>() == "realTime")
	{
		// 新线程，实时获取数据
		loader=thread([&]()
					  { threadLoadData(allTriggers, LigDataApi, rwMutex); });
	}


	std::this_thread::sleep_for(std::chrono::seconds(3));

	// GPSTime endTime = allTriggers.back().time;
	int idx = 0;
	for (const auto &x : allTriggers)
	{
		cout << CGPSTimeAlgorithm::GetTimeStr(x.time) << " " << x.stationID << endl;
		idx++;
		if (idx == 300)
			break;
	}

	// 开始定位
	thread locThread([&]()
					 { ThreadLoc(allTriggers, siteMap, siteTimeMap, rwMutex, config); });

	locThread.join();
	if (config["mode"].as<string>() == "realTime") loader.join();
	return 0;
}