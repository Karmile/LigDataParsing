#include <iostream>
#include <json/json.h>
#include <httplib.h>
#include "yaml-cpp/yaml.h"
#include"parse_json.h"



Parser::Parser(const std::string str) {
	try {
		config_ = YAML::LoadFile(str);
		std::cout << "load config successfully!\n" << endl;
	}
	catch (const YAML::Exception& e) {
		std::cerr << e.what() << "\n";
		return;
	}
}
// 定义结构体来存储站点信息
void Parser::parse_station_json(std::vector<StationInfo>& sites) {
	cout << config_["sites"]["url"].as<string>();
	httplib::Client client(config_["sites"]["url"].as<string>());
	auto res = client.Get(config_["sites"]["api"].as<string>());
	if (res && res->status == 200) {
		// 解析JSON响应
		JSONCPP_STRING err;
		Json::Value root;
		Json::Reader reader;
		//Json::Reader reader;
		const auto rawJsonLength = static_cast<int>(res->body.length());
		cout << res->body.c_str();
		try {
			if (reader.parse(res->body.c_str(), res->body.c_str() + rawJsonLength, root)) {
				// 遍历JSON数组并将站点信息存储到结构体中
				for (const auto& item : root) {
					StationInfo site;
					site.stationID = item["stationID"].asInt();
					site.latitude = item["latitude"].asDouble();
					site.name = static_cast<string>(item["name"].asString());
					site.longitude = item["longitude"].asDouble();
					site.altitude = item["altitude"].asDouble();
					//site.TotalDiskSpace = item["TotalDiskSpace"].asInt();
					//site.FreeDiskSpace = item["FreeDiskSpace"].asInt();
					site.gpsTime = item["GPSTime"].asString();
					site.id = item["id"].asInt();
					site.GPSIsValid = item["GPSIsValid"].asBool();
					sites.push_back(site);
				}
				// 打印获取的站点信息
				for (const auto& site : sites) {
					std::cout << "Name: " << site.name << std::endl;
					std::cout << "StationID: " << site.stationID << std::endl;
					std::cout << std::endl;
				}
			}
			else {
				std::cerr << "Failed to parse JSON response" << std::endl;
				std::cout << " :" << ":" << std::endl;
			}
		}
		catch (const Json::Exception& e) {
			// 捕获其他 Json::Exception 异常
			std::cout << "解析异常: " << e.what() << std::endl;
		}
	}
	else {
		std::cout << "HTTP request failed" << std::endl;
	}
}

void Parser::parse_trigger_json(std::vector<TriggerInfo>& triggers) {
	httplib::Client client(config_["trigger"]["url"].as<string>());
	auto res = client.Get(config_["trigger"]["api"].as<string>());
	if (res && res->status == 200) {
		Json::Value root;
		Json::Reader reader;
		vector<TriggerInfo> cache; // 中间程序
		// 解析 JSON 数据
		try {
			if (reader.parse(res->body.c_str(), root)) {
				for (const auto& item : root) {
					// 遍历JSON数组并将站点信息存储到结构体中
					TriggerInfo trigger;
					trigger.stationID = stoi(item[7].asString());
					//trigger.Mean = stoi(item[8].asString());
					trigger.Value = (stoi(item[9].asString()) - stoi(item[8].asString())) * 1.0 / 2048 * 5.0;
					trigger.time = GPSTime(stoi(item[0].asString()), stoi(item[1].asString()), stoi(item[2].asString()),
						stoi(item[3].asString()), stoi(item[4].asString()), stoi(item[5].asString()), stoi(item[6].asString()) * 1.0 / 1e9);
					cache.push_back(trigger);
				}
				cout << "The number of trigger info obtained: " << root.size();
			}
			else {
				std::cerr << "Failed to parse JSON response" << std::endl;
			}
		}
		catch (const Json::Exception& e) {
			// 捕获其他 Json::Exception 异常
			std::cout << "parsing fault!: " << e.what() << std::endl;
		}
		triggers.insert(triggers.end(), cache.begin(), cache.end());
	}
}

