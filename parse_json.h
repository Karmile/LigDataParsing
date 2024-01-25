#pragma once
#include "yaml-cpp/yaml.h"
#include <iostream>
#include "DataStruct.h"
#include <httplib.h>

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
	void Parser::parse_result(const httplib::Result& res, vector<TriggerInfo>& alltriggers);
	YAML::Node config_;

};

