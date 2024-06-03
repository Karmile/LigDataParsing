#pragma once
#define Pi 3.1415926
#define PI 3.1415926

//#define c 299792.458

#define BLOCK_X 32
#define BLOCK_Y 32
#define BLOCK_Z 1
#define E_R 6371.004
#define DATAPREC float
#define SAVE_PATH "./Err_Result.txt"

struct LocPara {
  DATAPREC Lat;
  DATAPREC boundW;
  DATAPREC boundE;
  DATAPREC boundS;
  DATAPREC boundN;
  DATAPREC boundhb;
  DATAPREC boundht;
  DATAPREC dh;
};

struct LocSta {
  LocSta(DATAPREC InLat, DATAPREC InLon, DATAPREC Inh, DATAPREC time = 0) {
    Lat = InLat;
    Lon = InLon;
    h = Inh;
    occur_t = time;
  }
  LocSta() {}
  DATAPREC Lat;
  DATAPREC Lon;
  DATAPREC h;
  DATAPREC sq;
  DATAPREC occur_t;
  DATAPREC confidence;
};