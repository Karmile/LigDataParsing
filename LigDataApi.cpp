#include <iostream>
#include <json/json.h>
#include "yaml-cpp/yaml.h"
#include "LigDataApi.h"
#include "chrono"

LigDataApi::LigDataApi(const std::string str) {
    try {
        config_ = YAML::LoadFile(str);
        std::cout << "load config successfully!\n"  << endl;
    }
    catch (const YAML::Exception& e) {
        std::cerr << e.what() << "\n";
        return ;
    }
}

// 定义结构体来存储站点信息
vector<StationInfo> LigDataApi::GetStationData(){
    vector<StationInfo> sites_;
    httplib::Client client(config_["sites"]["url"].as<string>());
    auto res = client.Get(config_["sites"]["api"].as<string>());
    if (res && res->status == 200) {
        // 解析JSON响应
        JSONCPP_STRING err;
        Json::Value root;
        Json::Reader reader;
		vector<TriggerInfo> cache; // 中间程序

        //Json::Reader reader;
        const auto rawJsonLength = static_cast<int>(res->body.length());
        try {
            if (reader.parse(res->body.c_str(), res->body.c_str() + rawJsonLength, root)) {
                sites_.clear();
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
					//site.id = item["id"].asInt();
					site.GPSIsValid = item["GPSIsValid"].asBool();
					sites_.push_back(site);
                }
                // 打印获取的站点信息
                for (const auto& site : sites_) {
                    std::cout << "Name: " << site.name;
                    std::cout << "   StationID: " << site.stationID << std::endl;
                }
                std::cout << "Sites info obtained from api:  " << sites_.size() << std::endl << endl;
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
    return sites_;
}

void LigDataApi::parse_result(const httplib::Result &res, vector<TriggerInfo>&alltriggers){
    if (res && res->status == 200) {
        Json::Value root;
        Json::Reader reader;
        // 解析 JSON 数据
        try {
            if (reader.parse(res->body.c_str(), root)) {
                for (const auto& item : root) {
                    // 遍历JSON数组并将站点信息存储到结构体中
                    TriggerInfo trigger;
                    trigger.stationID = stoi(item[7].asString());
                    //trigger.Mean = stoi(item[8].asString());
                    trigger.Value = (stof(item[9].asString()) - stof(item[8].asString())) * 1.0 / 2048 * 5.0;
                    trigger.time = GPSTime(stoi(item[0].asString()), stoi(item[1].asString()), stoi(item[2].asString()),
                        stoi(item[3].asString()), stoi(item[4].asString()), stoi(item[5].asString()), stof(item[6].asString()) / 1.0e9);
                    alltriggers.push_back(trigger);
                }
                cout << "Trigger info obtained from api: " << alltriggers.size() << endl << endl;
            }
            else {
                std::cerr << "Failed to parse JSON response" << std::endl;
            }
        }
        catch (const Json::Exception& e) {
            // 捕获其他 Json::Exception 异常
            std::cout << "parsing fault!: " << e.what() << std::endl;
        }

    }
}
vector<TriggerInfo> LigDataApi::GetTriggersData() {
    vector<TriggerInfo> allTriggers_;
    httplib::Client client(config_["trigger"]["url"].as<string>());
    auto mode = config_["mode"].as<string>();
    if (mode == "reProcess")
    {        
        GPSTime st_time(config_["reProcess"]["startTime"].as<string>());
        GPSTime end_time(config_["reProcess"]["endTime"].as<string>());
        while (st_time < end_time)
        {
            st_time += Duration("00000000T000700");
            auto res = client.Get(config_["trigger"]["api_nt"].as<string>() + st_time.str() + "/10");
            parse_result(res,allTriggers_);
        }
    }
    else if (mode == "realtime")
    {
        auto res = client.Get(config_["trigger"]["api"].as<string>());
        parse_result(res, allTriggers_);
    }
    return allTriggers_;
}

