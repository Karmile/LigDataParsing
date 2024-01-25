#pragma once
#include<string>
#include<cmath>
#include <iomanip>
#include <sstream>

using namespace std;

struct GPSTime
{
	GPSTime(void)
	{
		m_isTimeVaild = false;
		m_isTimeConfirm = false;
		m_Year = 0;
		m_Month = 0;
		m_Day = 0;
		m_Hour = 0;
		m_Min = 0;
		m_Sec = 0;
		m_ActPointSec = 0;
	}

	GPSTime(int input_Year, int input_Month, int input_Day, int input_Hour, int input_Min, int input_Sec, double input_ActPointSec, bool isTimeVaild = true, bool isTimeConfirm = true)
	{
		m_isTimeVaild = isTimeVaild;
		m_isTimeConfirm = isTimeConfirm;
		m_Year = input_Year;
		m_Month = input_Month;
		m_Day = input_Day;
		m_Hour = input_Hour;
		m_Min = input_Min;
		m_Sec = input_Sec;
		m_ActPointSec = input_ActPointSec;
	}
	GPSTime(string input_str)
	{
		if (input_str.find("T") != 0&&input_str.length()==15)
		{
			m_isTimeVaild = true;
			m_isTimeConfirm = true;
			m_Year  = stoi(input_str.substr(0, 4));
			m_Month = stoi(input_str.substr(4, 2));
			m_Day = stoi(input_str.substr(6, 2));
			m_Hour = stoi(input_str.substr(9, 2));
			m_Min = stoi(input_str.substr(11, 2));
			m_Sec = stoi(input_str.substr(13, 2));
			m_ActPointSec = 0;
		}
		else if (input_str.length() == 14)
		{
			m_isTimeVaild = true;
			m_isTimeConfirm = true;
			m_Year = stoi(input_str.substr(0, 4));
			m_Month = stoi(input_str.substr(4, 2));
			m_Day = stoi(input_str.substr(6, 2));
			m_Hour = stoi(input_str.substr(8, 2));
			m_Min = stoi(input_str.substr(10, 2));
			m_Sec = stoi(input_str.substr(12, 2));
			m_ActPointSec = 0;
		}
		else
		{
			m_isTimeVaild = false;
			m_isTimeConfirm = false;
			m_Year = 0;
			m_Month = 0;
			m_Day = 0;
			m_Hour = 0;
			m_Min = 0;
			m_Sec = 0;
			m_ActPointSec = 0;
		}

	}
	friend bool operator== (const GPSTime& s1, const GPSTime& s2)
	{
		return (s1.m_ActPointSec == s2.m_ActPointSec)
			&& (s1.m_Year == s2.m_Year)
			&& (s1.m_Month == s2.m_Month)
			&& (s1.m_Day == s2.m_Day)
			&& (s1.m_Hour == s2.m_Hour)
			&& (s1.m_Min == s2.m_Min)
			&& (s1.m_Sec == s2.m_Sec);
	}
	friend bool operator< (const GPSTime& s1, const GPSTime& s2)
	{
		return((s1.m_Year < s2.m_Year))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month < s2.m_Month))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day < s2.m_Day))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day == s2.m_Day) && (s1.m_Hour < s2.m_Hour))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day == s2.m_Day) && (s1.m_Hour == s2.m_Hour) && (s1.m_Min < s2.m_Min))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day == s2.m_Day) && (s1.m_Hour == s2.m_Hour) && (s1.m_Min == s2.m_Min) && (s1.m_Sec < s2.m_Sec))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day == s2.m_Day) && (s1.m_Hour == s2.m_Hour) && (s1.m_Min == s2.m_Min) && (s1.m_Sec == s2.m_Sec) && (s1.m_ActPointSec < s2.m_ActPointSec));
	}
	friend bool operator> (const GPSTime& s1, const GPSTime& s2)
	{
		return((s1.m_Year > s2.m_Year))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month > s2.m_Month))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day > s2.m_Day))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day == s2.m_Day) && (s1.m_Hour > s2.m_Hour))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day == s2.m_Day) && (s1.m_Hour == s2.m_Hour) && (s1.m_Min > s2.m_Min))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day == s2.m_Day) && (s1.m_Hour == s2.m_Hour) && (s1.m_Min == s2.m_Min) && (s1.m_Sec > s2.m_Sec))
			|| ((s1.m_Year == s2.m_Year) && (s1.m_Month == s2.m_Month) && (s1.m_Day == s2.m_Day) && (s1.m_Hour == s2.m_Hour) && (s1.m_Min == s2.m_Min) && (s1.m_Sec == s2.m_Sec) && (s1.m_ActPointSec > s2.m_ActPointSec));
	}
	friend bool operator!= (const GPSTime& s1, const GPSTime& s2)
	{
		return !((s1.m_ActPointSec == s2.m_ActPointSec)
			&& (s1.m_Year == s2.m_Year)
			&& (s1.m_Month == s2.m_Month)
			&& (s1.m_Day == s2.m_Day)
			&& (s1.m_Hour == s2.m_Hour)
			&& (s1.m_Min == s2.m_Min)
			&& (s1.m_Sec == s2.m_Sec));
	}

	friend double operator- (const GPSTime& s1, const GPSTime& s2)
	{
		if ((s1.m_Year == s2.m_Year)
			&& (s1.m_Month == s2.m_Month)
			&& (s1.m_Day == s2.m_Day)
			&& (s1.m_Hour == s2.m_Hour)
			&& (s1.m_Min == s2.m_Min)
			&& (s1.m_Sec == s2.m_Sec)){
				return s1.m_ActPointSec - s2.m_ActPointSec;
			}
		return (s1.m_Year - s2.m_Year)*1e10+(s1.m_Month - s2.m_Month)*1e8+(s1.m_Day - s2.m_Day)*1e6+(s1.m_Hour - s2.m_Hour)*1e4+(s1.m_Min - s2.m_Min)*1e2+(s1.m_Sec - s2.m_Sec)+s1.m_ActPointSec - s2.m_ActPointSec;
	}

	GPSTime operator+= (const GPSTime& s2)
	{
		GPSTime res;
		m_Sec = (m_Sec + s2.m_Sec) % 60;
		int carrySecs = (m_Sec + s2.m_Sec) / 60;
		m_Min = (m_Min + s2.m_Min + carrySecs) % 60;
		int carryMins = (m_Min + s2.m_Min + carrySecs) / 60;
		m_Hour = (m_Hour + s2.m_Hour + carryMins) % 24;
		int carryHours = (m_Hour + s2.m_Hour + carryMins) / 24;
		// 计算日期，并考虑月底和年底的特殊情况
		int maxDays = 31;
		if (m_Month == 2)
		{
			maxDays = (m_Year % 4 == 0 && m_Year % 100 != 0) || m_Year % 400 == 0 ? 29 : 28;
		}
		else if (m_Month == 4 || m_Month == 6 || m_Month == 9 || m_Month == 11)
		{
			maxDays = 30;
		}
		m_Day = (m_Day + s2.m_Day + carryHours) % (maxDays + 1);
		int carryDays = (m_Day + s2.m_Day + carryHours) / (maxDays + 1);

		m_Month = (m_Month + s2.m_Month + carryDays - 1) % 12 + 1;
		int carryMonths = (m_Month + s2.m_Month + carryDays - 1) / 12;

		m_Year = m_Year + s2.m_Year + carryMonths;
		return *this;
	}
	string str() const {
		char buffer[20];
		std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
			m_Year, m_Month, m_Day, m_Hour, m_Min, m_Sec);
		return std::string(buffer);
	}
	
	bool m_isTimeVaild;
	bool m_isTimeConfirm;
	int m_Year;
	int m_Month;
	int m_Day;
	int m_Hour;
	int m_Min;
	int m_Sec;
	double m_ActPointSec;
};

struct StationInfo {
  //int id;
  int stationID;
  string name;
  double latitude;
  double longitude;
  double altitude;
  string gpsTime;
  bool GPSIsValid;
  //int TotalDiskSpace;
  //int FreeDiskSpace;
  
  //double distance(const StationInfo& s1, const StationInfo& s2)
  //{
	 //return Stadistance(s1.latitude, s1.longitude, s2.latitude, s2.longitude);
  //}
};

struct TriggerInfo {
	GPSTime time;
	int stationID;
	double Value;
	int staIndex;
	friend bool operator<(const TriggerInfo& s1, const TriggerInfo& s2)
	{
		return (s1.time) < (s2.time) || (s1.time == s2.time && s1.stationID < s2.stationID);
	}

	friend bool operator>(const TriggerInfo& s1, const TriggerInfo& s2)
	{
		return (s1.time) > (s2.time) || (s1.time == s2.time && s1.stationID > s2.stationID);
	}

	friend bool operator== (const TriggerInfo& s1, const TriggerInfo& s2)
	{
		return (s1.time == s2.time)
			&& (s1.stationID == s2.stationID);
	}

	friend bool operator!= (const TriggerInfo& s1, const TriggerInfo& s2)
	{
		return (s1.time != s2.time)
			|| (s1.stationID != s2.stationID);
	}
};


struct GPSTimeInformation {
	GPSTimeInformation()
	{
		memset(m_currentTimeStr, 0, 256);
		m_timeIsProcess = false;
		m_currentTime = GPSTime();
		m_currentSecondCacheCount = 0;
		m_currentSeconfCachePlace = 0;
	}
	GPSTime m_currentTime;
	char m_currentTimeStr[256];
	unsigned int m_currentSecondCacheCount;
	unsigned int m_currentSeconfCachePlace;

	bool m_timeIsProcess;  //cannot use the GPS time, When this parameter is on, you could wait;
};

struct DiskState
{
	double   TotalDiskSpace;
	double   FreeDiskSpace;
	double   UsedDiskSpace;
};

struct ClientState
{
	int StationID;
	char stationName[32];

	GPSTimeInformation CurrentGPSTimeInformation;
	GPSTime GPSCurrentTime;

	double GPSCurrentLocationLat;
	double GPSCurrentLocationLon;

	bool  detectionThreadIsOn;
	bool  GPSSearchThreadIsOn;
	bool  fileProcThreadIsOn;
	bool  readPortThreadIsOn;
	bool  GPSSynchronizationThreadIsOn;

	bool isDAQStart;

	int   GPSSynchronizationErrorFlagCount;
	int   GPSReadPortErrorFlagCount;
	double GPSPointsBetweenTwoEvent;

	unsigned int GPSSecondCacheCount;
	unsigned int GPSSecondCachePlace;

	DiskState diskState;
};

struct TriggerPointInformation
{
	int StationID;
	GPSTime PointTime;
	unsigned int Mean;
	unsigned int Value;
};