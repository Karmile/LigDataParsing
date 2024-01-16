#pragma once
#include "yaml-cpp/yaml.h"
#include <iostream>
#include "DataStruct.h"

class Parser
{
public:
	Parser(const std::string str );

	void parse_station_json(std::vector<StationInfo>& sites);
	void parse_trigger_json(std::vector<TriggerInfo>& triggers);
private:
	YAML::Node config_;
};

