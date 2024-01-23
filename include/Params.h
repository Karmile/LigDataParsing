// Define of common used parameters.
#pragma once
#include "DataStruct.h"

#define FTYPE double
#define PI 3.1415926535897931
#define degree2radians 0.017453292519943
#define cVeo 299792.458

#define R_OK 4 /* Test for read permission.  */
#define W_OK 2 /* Test for write permission.  */
//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK 0 /* Test for existence.  */

#define Lattice 0.001
#define boundaryW 111.5
#define boundaryE (boundaryW + (448) * 0.01)
#define boundaryS 21.5
#define boundaryN (boundaryS + (448) * 0.01)

using namespace std;

//static string configFileName = "../static/config.json";
//static string configFileName = "./config.json";

//static unsigned long long CountGeoLocationTimes = 0;
//static unsigned long long CountValidLocations = 0;

//static string curOutPutFileName;
static GPSTime currentFileT;
static int cachePoolLimitEachStation = 3;
static double sqLevel = 0.3;
static GPSTime pastFileTime;
static unsigned long long CountGeoLocationTimes = 0;
