#pragma once
#include "LigDataApi.h"
#include "GPSTimeAlgorithm.h"
#include "Params.h"
#include "LocStruct.h"
#include <thread>
#include <map>
#include <shared_mutex>
#include <chrono>
#include "WorkThreads.h"
#include "LigTools.h"

using namespace std;

// The thread of load data.
void threadLoadData(vector<TriggerInfo>& allTriggers, LigDataApi& LigDataApi, shared_mutex& rwMutex, bool isReProcess = false);
void ThreadLoc( vector<TriggerInfo> &allTriggers, map<int, StationInfo> &siteMap, map<int, map<int, double>> &siteTimeMap, shared_mutex &rwMutex, YAML::Node config);
