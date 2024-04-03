#pragma once
#include <chrono>
#include <map>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "./include/threadpool.h"
#include "GPSTimeAlgorithm.h"
#include "LigDataApi.h"
#include "LigTools.h"
#include "LocStruct.h"
#include "Params.h"
using namespace std;

// The thread of load data.
void threadLoadData(deque<TriggerInfo> &allTriggers, LigDataApi &LigDataApi, shared_mutex &rwMutex,
                    YAML::Node &config, bool &keep_loading);
void ThreadLoc(deque<TriggerInfo> &allTriggers, deque<TriggerInfo> &transTriggers,
               unordered_map<int, StationInfo> &siteMap,
               unordered_map<int, unordered_map<int, double>> &siteTimeMap, shared_mutex &rwMutex,
               YAML::Node config, bool &keep_loading);
