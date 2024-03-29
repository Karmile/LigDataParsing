#include "LigDataApi.h"

#include <json/json.h>

#include <iostream>

#include "GPSTimeAlgorithm.h"
#include "LigTools.h"
#include "chrono"
#include "yaml-cpp/yaml.h"

YAML::Node LigDataApi::config_ = YAML::LoadFile(".\\config.yaml");
LigDataApi::LigDataApi(const std::string str) {
  try {
    config_ = YAML::LoadFile(str);
    LOG_INFO("load config successfully!\n" << endl);
  } catch (const YAML::Exception& e) {
    LOG_ERROR(e.what() << "\n" << endl);
    return;
  }
}

// 定义结构体来存储站点信息
vector<StationInfo> LigDataApi::GetStationData() {
  vector<StationInfo> sites_;
  httplib::Client client(config_["sites"]["url_u"].as<string>());
  httplib::Result res;

  for (int i = 0; i < 6; i++) {
    if (i < 3) {
      res = client.Get(config_["sites"]["api"].as<string>());
    } else {
      res = httplib::Client(config_["sites"]["url_j"].as<string>())
                .Get(config_["sites"]["api"].as<string>());
      LOG_WARN("try url_j sites api!" << endl);
    }
    if (res && res->status == 200) {
      break;
    } else {
      LOG_WARN("Request url failed. Reconnecting: " << i + 1 << endl);
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
  }
  if (res && res->status == 200) {
    // 解析JSON响应
    JSONCPP_STRING err;
    Json::Value root;
    Json::Reader reader;
    vector<TriggerInfo> cache;  // 中间程序

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
          if (site.stationID < 1000) sites_.emplace_back(site);
        }
        // 打印获取的站点信息
        for (const auto& site : sites_) {
          LOG_INFO("Name: " << site.name <<"  StationID: " << site.stationID << endl);
        }
        LOG_INFO("Sites info obtained from api:  " << sites_.size() << endl);
      } else {
        LOG_ERROR("Failed to parse JSON response" << endl);
      }
    } catch (const Json::Exception& e) {
      // 捕获其他 Json::Exception 异常
      LOG_WARN("解析异常: " << e.what() << endl);
    }
  } else {
    LOG_WARN("HTTP request failed" << endl);
  }
  return sites_;
}

void LigDataApi::parse_result(const httplib::Result& res, vector<TriggerInfo>& alltriggers) {
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
          trigger.time =
              GPSTime(stoi(item[0].asString()) - 2000, stoi(item[1].asString()),
                      stoi(item[2].asString()), stoi(item[3].asString()), stoi(item[4].asString()),
                      stoi(item[5].asString()), stof(item[6].asString()) / 1.0e9);
          alltriggers.emplace_back(trigger);
        }
        LOG_INFO("Trigger info obtained from api: " << alltriggers.size() << endl);
      } else {
        LOG_ERROR("Failed to parse JSON response" << endl);
      }
    } catch (const Json::Exception& e) {
      // 捕获其他 Json::Exception 异常
      LOG_ERROR("parsing fault!: " << e.what() << endl);
    }
  }
}

vector<TriggerInfo> LigDataApi::GetRealTimeTriggerData() {
  vector<TriggerInfo> allTriggers_;
  //allTriggers_.reserve(1000000);
  httplib::Client client(config_["trigger"]["url_u"].as<string>());
  httplib::Result res;
  for (int i = 0; i < 6; i++) {
    if (i < 3) {
      res = client.Get(config_["trigger"]["api_rt"].as<string>());

    } else {
      res = httplib::Client(config_["trigger"]["url_j"].as<string>())
                .Get(config_["trigger"]["api_rt"].as<string>());
      LOG_WARN("try url_j trigger api!" << endl);
    }
    if (res && res->status == 200) {
      break;
    } else {
      LOG_WARN("Request url failed. Reconnecting: " << i + 1 << endl);
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
  }

  parse_result(res, allTriggers_);
  sort(allTriggers_.begin(), allTriggers_.end());  // 按照时间排序
  allTriggers_.erase(unique(allTriggers_.begin(), allTriggers_.end()), allTriggers_.end());  // 去重
  return allTriggers_;
}

vector<TriggerInfo> LigDataApi::GetHistoricalTriggerDataUntill(GPSTime TillTime, int Minutes) {
  vector<TriggerInfo> allTriggers_;
  //allTriggers_.reserve(1000000);
  httplib::Client client(config_["trigger"]["url_u"].as<string>());
  httplib::Result res;

  for (int i = 0; i < 6; i++) {
    if (i < 3) {
      res = client.Get(config_["trigger"]["api_nt"].as<string>() +
                       TillTime.str().replace(0, 2, "20") + "/" + to_string(Minutes));
    } else {
      res = httplib::Client(config_["trigger"]["url_j"].as<string>())
                .Get(config_["trigger"]["api_nt"].as<string>() +
                     TillTime.str().replace(0, 2, "20") + "/" + to_string(Minutes));

      LOG_INFO("try url_j trigger api!" << endl);
    }
    if (res && res->status == 200) {
      break;
    } else {
      LOG_WARN("Request url failed. Reconnecting: " << i + 1 << endl);
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
  }
  parse_result(res, allTriggers_);

  sort(allTriggers_.begin(), allTriggers_.end());  // 按照时间排序
  allTriggers_.erase(unique(allTriggers_.begin(), allTriggers_.end()), allTriggers_.end());  // 去重
  return allTriggers_;
}

void LigDataApi::PostLigResult(const GPSTime lig_time, const LocSta res,
                               const std::vector<TriggerInfo> oneComb,
                               std::unordered_map<int, StationInfo>& siteMap) {
  httplib::Client client(config_["ligresult"]["url"].as<string>());
  string total_names;
  string total_IDs;
  string total_infos;
  std::stringstream ss;
  GPSTime min_t{oneComb[0].time};
  min_t += Duration("00000000T000100");
  int min_idx{0};
  double min_dis{0.0};
  double F{0.0};
  double residual{0.0};
  for (size_t i = 0; i < oneComb.size(); ++i) {
    const auto& iter = oneComb[i];
    const auto& site = siteMap[iter.stationID];
    total_IDs += std::to_string(iter.stationID) + "+";
    total_names += site.name + "+";
    double dis{
        Stadistance_3D(res.Lat, res.Lon, res.h, site.latitude, site.longitude, site.altitude)};
    residual = (iter.time - lig_time) * cVeo - dis;
    ss << "{"
       << "\"id\":" << iter.stationID << ","
       << "\"name\":\"" << site.name << "\","
       << "\"value\":" << iter.Value << ","
       << "\"peakCurrent\":"
       << "\" \""
       << ","
       << "\"residual\":" << residual << "},";
    if (iter.time < min_t && config_["gainCoefficient"][oneComb[i].stationID]) {
      min_t = iter.time;
      min_idx = i;
      F = config_["gainCoefficient"][oneComb[min_idx].stationID].as<double>();
      min_dis = dis;
    }
  }
  const auto& min_sta = siteMap[oneComb[min_idx].stationID];
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
  data["peakCurrent"] = oneComb[min_idx].Value * min_dis * F;
  data["type"] = (res.h < 2.0) ? "RS" : "IC";
  data["datetime"] = lig_time.str().replace(0, 2, "20");
  data["nameOfSta"] = total_names;
  data["IDOfSta"] = total_IDs;
  data["infoOfSta"] = total_infos;
  data["isDeleted"] = 0;
  data["locationMethod"] = "TOA";

  // 将Json::Value对象转换为JSON格式的字符串
  Json::StreamWriterBuilder builder;
  builder["commentStyle"] = "None";
  builder["indentation"] = "   ";  // 设置缩进

  // Write to .loc file
  FILE* fp = fopen(
      LigTools::GetLocFileName(config_["OutputFilePath"].as<string>(), lig_time).c_str(), "ab");
  if (fp) {
    std::string str = CGPSTimeAlgorithm::GetTimeString(lig_time) + " " + std::to_string(res.Lat) +
                      " " + std::to_string(res.Lon) + " " + std::to_string(res.h) + " " +
                      std::to_string(res.sq) + " " + std::to_string(oneComb.size()) + " " +
                      data["peakCurrent"].asString();

    str += " " + data["type"].asString() + " " + data["locationMethod"].asString() + " " +
           total_names + " " + total_IDs;

    str += "\r\n";
    fwrite(str.c_str(), 1, str.length(), fp);
    fclose(fp);
  }

  std::string json_data = Json::writeString(builder, data);
  int max_retries = 3;
  int retry_count = 0;
  int retry_delay_seconds = 1;

  while ((retry_count < max_retries) && (config_["OutputDataBase"].as<bool>() == true)) {
    auto result =
        client.Post(config_["ligresult"]["api"].as<string>() + "/1", json_data, "application/json");

    if (result && result->status == 200) {
      // 请求成功
      break;  // 跳出重试循环
    } else {
      // 请求失败，输出错误信息
      LOG_WARN("Request failed. result->status: " << result->status << endl);

      // 增加重试计数
      retry_count++;

      // 延迟一段时间后进行重试
      std::this_thread::sleep_for(std::chrono::seconds(retry_delay_seconds));
    }
  }

  if (retry_count == max_retries) {
    LOG_ERROR("Max retries reached. Unable to connect to the server." << endl);
  }
}
