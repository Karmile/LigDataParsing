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
  static void connect(); 
  static void disconnect(); 
 private:
  
  static void sendDataViaMQTT(const std::string& data);
  void LigDataApi::parse_result(const httplib::Result& res, vector<TriggerInfo>& alltriggers);
  static YAML::Node config_;
};
const std::string serverURI = "tcp://36.7.72.128:18830";  // MQTT 代理地址
const std::string clientId = "LDP-Sferic";                    // MQTT 客户端 ID
const std::string topic = "ltg-beta";                      // MQTT 主题
const std::string username = "lig_beta";                   // MQTT 代理用户名
const std::string password = "lig_beta";                   // MQTT 代理密码
static mqtt::client client_(serverURI, clientId);