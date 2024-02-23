#pragma once
#include "LocStruct.h"
#include <map>
#include "LocStruct.h"
#include <unordered_map>

using namespace std;

class LigTools
{
    LigTools();
	virtual ~LigTools();

public:
    static vector<vector<TriggerInfo>> getLocationPool(unordered_map<int, triggerAtStation>& triggerPool);
    static void permuteVector(vector<vector<TriggerInfo>>triggers, vector<TriggerInfo>& current, int index, vector<vector<TriggerInfo>>& CombinationPool, unordered_map<int, unordered_map<int, double>>& siteTimeMap);
    static vector<vector<TriggerInfo>> getLocationPool_p(unordered_map<int, triggerAtStation>& triggerPool, unordered_map<int, unordered_map<int, double>>& siteTimeMap, int nCombStas);
    static bool check_location_structure(const vector<LocSta>& Stations, LocSta& result, double checkTheta);
};

