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
#include "./include/threadpool.h"
#include <unordered_map>

using namespace std;

// The thread of load data.
void threadLoadData(deque<TriggerInfo>& allTriggers, LigDataApi& LigDataApi, shared_mutex& rwMutex, YAML::Node& config, bool isReProcess);
void ThreadLoc(deque<TriggerInfo> &allTriggers, deque<TriggerInfo> &transTriggers, unordered_map<int, StationInfo> &siteMap, unordered_map<int, unordered_map<int, double>> &siteTimeMap, shared_mutex &rwMutex, YAML::Node config);
