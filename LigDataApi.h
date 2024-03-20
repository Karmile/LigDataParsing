#pragma once
#include <httplib.h>

#include <iostream>
#include <unordered_map>

#include "./ThirdParty/LigLocCuda3Dx64/include/LocCuda.h"
#include "DataStruct.h"
#include "yaml-cpp/yaml.h"

class LigDataApi {
 public:
  LigDataApi(const std::string str);
  vector<StationInfo> GetStationData();
  vector<TriggerInfo> GetHistoricalTriggerDataUntill(GPSTime TillTime, int Minutes);
  vector<TriggerInfo> GetRealTimeTriggerData();
  static void PostLigResult(const GPSTime lig_time, const LocSta res,
                            const std::vector<TriggerInfo> oneComb,
                            std::unordered_map<int, StationInfo>& siteMap);

 private:
  void LigDataApi::parse_result(const httplib::Result& res, vector<TriggerInfo>& alltriggers);
  static YAML::Node config_;
};
