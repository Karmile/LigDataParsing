#pragma once
#include "yaml-cpp/yaml.h"
#include <iostream>
#include "DataStruct.h"
#include <httplib.h>

class LigDataApi
{
public:
	LigDataApi(const std::string str );
	vector<StationInfo> GetStationData();
	vector<TriggerInfo> GetTriggersData();

private:
	void LigDataApi::parse_result(const httplib::Result& res, vector<TriggerInfo>& alltriggers);
	YAML::Node config_;
};

