// Define of common used parameters.
#pragma once
#include "DataStruct.h"

#define FTYPE double
#define PI 3.1415926535897931
#define degree2radians 0.017453292519943
#define radians2degree 57.32484076433121
#define cVeo 299792.458  // 299792.458 * 0.998
#define R 6371.004
#define FLOAT_MAX 1000000000000
#define Lattice 0.001
#define boundaryW 80.0
#define boundaryE (boundaryW + (6144) * 0.01)
#define boundaryS 0
#define boundaryN (boundaryS + (6144) * 0.01)

using namespace std;

//static string configFileName = "../static/config.json";
//static string configFileName = "./config.json";

//static unsigned long long CountGeoLocationTimes = 0;
//static unsigned long long CountValidLocations = 0;

//static string curOutPutFileName;
static int cachePoolLimitEachStation = 3;
static GPSTime pastFileTime;
static unsigned long long CountGeoLocationTimes = 0;
static unsigned long long CountLocationPoints = 0;
static unsigned long long AccumulatedIdleTime = 0;
struct Bounds {
  Bounds(double boundW_, double boundE_, double boundS_, double boundN_, double boundhb_,
         double boundht_) {
    boundW = boundW_ * degree2radians;
    boundE = boundE_ * degree2radians;
    boundS = boundS_ * degree2radians;
    boundN = boundN_ * degree2radians;
    boundhb = boundhb_;
    boundht = boundht_;
  }
  double boundW;
  double boundE;
  double boundS;
  double boundN;
  double boundhb;
  double boundht;
};
