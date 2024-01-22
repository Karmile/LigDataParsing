#pragma once
#include "yaml-cpp/yaml.h"
#include <iostream>
#include "DataStruct.h"

class Parser
{
public:
	Parser(const std::string str );
	//inline const vector<StationInfo>& GetStationData() {
	//	return sites_;
	//}
	//inline const vector<TriggerInfo>& GetTriggersData() {
	//	return allTriggers_;
	//}
	vector<StationInfo> GetStationData();
	vector<TriggerInfo> GetTriggersData();

private:
	YAML::Node config_;

};

