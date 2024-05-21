#pragma once
#include <httplib.h>

#include <iostream>
#include <unordered_map>

#include "./ThirdParty/LigLocCuda3Dx64/include/LocCuda.h"
#include "DataStruct.h"
#include "yaml-cpp/yaml.h"
#include "log.h"
#include <mqtt/client.h>

class LigDataApi {
 public:
  LigDataApi(const std::string str);
  vector<StationInfo> GetStationData();
  vector<TriggerInfo> GetHistoricalTriggerDataUntill(GPSTime TillTime, int Minutes);
  vector<TriggerInfo> GetRealTimeTriggerData();
  static void PostLigResult(const GPSTime lig_time, const LocSta res,
                            const std::vector<TriggerInfo> oneComb,
                            std::unordered_map<int, StationInfo>& siteMap);
  static void sendDataViaMQTT(const std::string& topic, const std::string& data);
  static void connect(); 
  static void disconnect(); 
 private:
  
  void LigDataApi::parse_result(const httplib::Result& res, vector<TriggerInfo>& alltriggers);
  static YAML::Node config_;
  static mqtt::client_ptr client_;
};
const std::string topic_l = "ltg-beta";                    // MQTT 闪电数据主题
const std::string topic_w = "warning/LDP";                 // MQTT 报警主题
const std::string username = "lig_beta";                   // MQTT 代理用户名
const std::string password = "lig_beta";                   // MQTT 代理密码

