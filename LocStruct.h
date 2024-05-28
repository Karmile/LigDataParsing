#pragma once
#include <iostream>
#include <vector>

#include "./ThirdParty/LigLocCuda3Dx64/include/LocCuda.h"
#include "DataStruct.h"
#include "GeoTools.h"

using namespace std;

class triggerElement {
 public:
  double m_time;
  int m_idx;
  double m_peak;
  friend bool operator<(const triggerElement &s1, const triggerElement &s2) {
    return abs(s1.m_peak) < abs(s2.m_peak);
  }

  friend bool operator>(const triggerElement &s1, const triggerElement &s2) {
    return abs(s1.m_peak) > abs(s2.m_peak);
  }

  triggerElement(double m_time, int m_idx, double m_peak)
      : m_time(m_time), m_idx(m_idx), m_peak(m_peak) {}
};

class triggerAtStation {
 public:
  vector<TriggerInfo> triggers;
  LocSta staLocation;
  int stationID;
  string staName;
};

struct OneLoc {
  GPSTime tt;
  LocSta locResult;
  int nSta;
  string staString;
  OneLoc(GPSTime tt, LocSta locResult, int nSta, string staString)
      : tt(tt), locResult(locResult), nSta(nSta), staString(staString) {}
};

struct stationInfomation {
  int id;
  string name;
  double peakValue;
  double peakI;
};

struct locationInformation {
  GPSTime time;
  string timeString;
  double latitude;
  double longtiude;
  double alitude;
  double sq;
  int numberOfStation;
  double peakCurrent;
  string type;
  string methodOfLocation;
  string IDOfStations;
  string nameOfStaions;
  vector<stationInfomation> stationInfos;
};

// Final location in CUDA Loc
LocSta FinalGeoLocation_GPU(vector<LocSta> Stations, vector<double> Loc_Time, LocSta result);

// Initial location in CUDA Loc
LocSta GeoLocation_GPU_Initial(vector<LocSta> Stations, vector<double> Loc_Time);
LocSta GeoLocation_GPU(vector<LocSta> Stations, vector<double> Loc_Time);
LocSta GeoLocation_OP(vector<LocSta> Stations, vector<double> Loc_Time,
                      LocSta initResult = LocSta(0.0, 0.0, 0.0), int num_threads = 1);
LocSta GeoLocation_OP_2(vector<LocSta> Stations, vector<double> Loc_Time,
                        LocSta initResult = LocSta(0.0, 0.0, 0.0), int num_threads = 1);
void CalculateGDOP(const vector<LocSta> &stations, LocSta &result);