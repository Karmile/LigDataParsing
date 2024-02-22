#include <iostream>
#include <json/json.h>
#include "yaml-cpp/yaml.h"
#include "LigDataApi.h"
#include "chrono"
#include "GPSTimeAlgorithm.h"

YAML::Node LigDataApi::config_ = YAML::LoadFile(".\\config.yaml");
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
    else if (mode == "realTime")
    {
        auto res = client.Get(config_["trigger"]["api_rt"].as<string>());
        parse_result(res, allTriggers_);
    }
    return allTriggers_;
}

void LigDataApi::PostLigResult(const GPSTime lig_time, const LocSta res, const std::vector<TriggerInfo> oneComb, std::map<int, StationInfo>& siteMap) {
    httplib::Client client(config_["ligresult"]["url"].as<string>());
    string total_names;
    string total_IDs;
    string total_infos;
    std::stringstream ss;
    for (const auto& iter : oneComb)
    {
        total_IDs += std::to_string(iter.stationID) + "+";
        total_names += siteMap[iter.stationID].name + "+";
        ss << "{"
            << "\"id\":" << iter.stationID << ","
            << "\"name\":\"" << siteMap[iter.stationID].name << "\","
            << "\"value\":" << iter.Value << ","
            << "\"peakCurrent\":" << "  "
            << "},";
    }
    total_infos = ss.str();
    // 去掉最后一个 "+" 和 ","字符串
    if (!oneComb.empty()) {
        total_IDs = total_IDs.substr(0, total_IDs.size() - 1);
        total_names = total_names.substr(0, total_names.size() - 1);
        total_infos = total_infos.substr(0, total_infos.size() - 1);
        total_infos = "[" + total_infos + "]";
    }

    Json::Value data;
    data["ExpTag"] = config_["expTag"].as<string>();
    data["time"] = CGPSTimeAlgorithm::GetTimeStr(lig_time);
    data["nano"] = lig_time.m_ActPointSec * 1e+9;
    data["latitude"] = res.Lat;
    data["longitude"] = res.Lon;
    data["altitude"] = res.h;
    data["residual"] = res.sq;
    data["numOfSta"] = oneComb.size();
    data["peakCurrent"] = " ";
    data["type"] = (res.h < 2.0) ? "RS" : "IC";
    data["datetime"] = lig_time.str();
    data["nameOfSta"] = total_names;
    data["IDOfSta"] = total_IDs;
    data["infoOfSta"] = total_infos;
    data["isDeleted"] = 0;
    data["locationMethod"] = "TOA";

    // 将Json::Value对象转换为JSON格式的字符串
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";  // 设置缩进

    std::string json_data = Json::writeString(builder, data);
    // 发送 POST 请求，并设置请求头和 JSON 数据
    auto start = std::chrono::high_resolution_clock::now();
    auto result = client.Post(config_["ligresult"]["api"].as<string>() + "/1", json_data, "application/json");
    auto end = std::chrono::high_resolution_clock::now();
    // 计算经过的时间（以秒为单位）
    double elapsed_seconds = std::chrono::duration<double>(end - start).count();

    if (result && result->status == 200) {
        std::cout << "Request succeeded. Response: " << result->body;
    }
}
