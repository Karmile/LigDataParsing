#include<string>

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
  int id;
  int stationID;
  string name;
  double latitude;
  double longitude;
  double altitude;
  string gpsTime;
  bool GPSIsValid;
  int TotalDiskSpace;
  int FreeDiskSpace;
  bool isDeleted;
};
struct TriggerInfo {
	GPSTime time;
	int ActPointSec;
	int stationID;
	int Mean;
	int Value;
};