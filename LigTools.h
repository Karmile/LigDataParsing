#pragma once
#include "LocStruct.h"
#include <map>
#include <unordered_map>
#include "include/GPSTimeAlgorithm.h"
#include "include/ordered_map.h"

using namespace tsl;
using namespace std;

class LigTools
{
    LigTools();
	virtual ~LigTools();

public:
    static vector<vector<TriggerInfo>> getLocationPool(ordered_map<int, triggerAtStation>& triggerPool);
    static void permuteVector(vector<vector<TriggerInfo>>triggers, vector<TriggerInfo>& current, int index, vector<vector<TriggerInfo>>& CombinationPool, unordered_map<int, unordered_map<int, double>>& siteTimeMap);
    static vector<vector<TriggerInfo>> getLocationPool_p(ordered_map<int, triggerAtStation>& triggerPool, unordered_map<int, unordered_map<int, double>>& siteTimeMap, int nCombStas);
    static bool check_location_structure(const vector<LocSta>& Stations, LocSta& result, double checkTheta);
    static string GetLocFileName(string filePath, const GPSTime Time);

};

