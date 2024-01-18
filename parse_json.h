#pragma once
#include "yaml-cpp/yaml.h"
#include <iostream>
#include "DataStruct.h"

class Parser
{
public:
	Parser(const std::string str );
	void Parse(bool get = true);
	inline const vector<StationInfo>& GetStationData() {
		return sites_;
	}
	inline const vector<TriggerInfo>& GetTriggersData() {
		return allTriggers_;
	}
private:
	YAML::Node config_;
	vector<StationInfo> sites_;
	vector<TriggerInfo> allTriggers_; 
	void parse_station_json();
	void parse_trigger_json();
};

