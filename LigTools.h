#pragma once
#include "LocStruct.h"
#include <map>
#include "LocStruct.h"

using namespace std;

class LigTools
{
    LigTools();
	virtual ~LigTools();

public:
    static vector<vector<TriggerInfo>> getLocationPool(map<int, triggerAtStation>& triggerPool);
    static void permuteVector(vector<vector<TriggerInfo>>triggers, vector<TriggerInfo>& current, int index, vector<vector<TriggerInfo>>& CombinationPool, map<int, map<int, double>>& siteTimeMap);
    static vector<vector<TriggerInfo>> getLocationPool_p(map<int, triggerAtStation>& triggerPool, map<int, map<int, double>>& siteTimeMap, int nCombStas);
    static bool check_location_structure(const vector<LocSta>& Stations, LocSta& result, double checkTheta);
};

