#pragma once
#include "yaml-cpp/yaml.h"
#include <iostream>
#include "DataStruct.h"
#include <httplib.h>
#include "./ThirdParty/LigLocCuda3Dx64/include/LocCuda.h"

class LigDataApi
{
public:
	LigDataApi(const std::string str );
	vector<StationInfo> GetStationData();
	vector<TriggerInfo> GetTriggersData();
	static void PostLigResult(const GPSTime &lig_time, const LocSta &res,const std::vector<TriggerInfo>& oneComb,std::map<int, StationInfo>&siteMap);

private:
	void LigDataApi::parse_result(const httplib::Result& res, vector<TriggerInfo>& alltriggers);
	static YAML::Node config_;
};

