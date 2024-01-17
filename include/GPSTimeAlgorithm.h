#pragma once
#include "DataStruct.h"
#include <math.h>
#include <string>
#include "stdio.h"
#include <ctime>
#include <memory>
#include <iostream>

using namespace std;

class CGPSTimeAlgorithm
{
public:
	CGPSTimeAlgorithm(void);

	static GPSTime CalculateActuallyTime(GPSTimeInformation OneGPSTime, unsigned long Count, unsigned long Place, unsigned long NumOfDateOfPerCache, double SampleRate)
	{
		GPSTime ActTime = OneGPSTime.m_currentTime;

		double Dvalue = ((double)Count - (double)OneGPSTime.m_currentSecondCacheCount) * (double)NumOfDateOfPerCache + ((double)Place - (double)OneGPSTime.m_currentSeconfCachePlace);

		return CalculteTime(ActTime, Dvalue, SampleRate);
	}

	static GPSTime CalculteTime(GPSTime ActTime, double Dvalue, double SampleRate)
	{
		ActTime.m_ActPointSec += Dvalue / SampleRate;
		if ((ActTime.m_ActPointSec >= 1) || (ActTime.m_ActPointSec < 0))
			return GPSTimeCarrtOneSecond(ActTime);
		else
			return ActTime;
	}

	/*
	static GPSTime GPSTimeCarrtOneSecond(GPSTime OneTime) //If the Point data of GPS >1, the time should be carried;
	{
		if(OneTime.m_ActPointSec>=1)
		{
			int DSeconds=(int)(OneTime.m_ActPointSec/1);
			OneTime.m_ActPointSec=fmod(OneTime.m_ActPointSec,1);
			OneTime.m_Sec+=DSeconds;

			if(OneTime.m_Sec>=60)
			{
				int DMinutes=OneTime.m_Sec/60;
				OneTime.m_Sec=OneTime.m_Sec%60;
				OneTime.m_Min+=DMinutes;
				if(OneTime.m_Min>=60)
				{
					int DHours=OneTime.m_Min/60;
					OneTime.m_Min=OneTime.m_Min%60;
					OneTime.m_Hour+=DHours;
					if(OneTime.m_Hour>=24)
					{
						int DDays=OneTime.m_Hour/60;
						OneTime.m_Hour=OneTime.m_Hour%60;
						OneTime.m_Day+=DDays;
					}
				}
			}

			return OneTime;
		}
		else if(OneTime.m_ActPointSec<0)
		{
			int DSeconds=(int)(OneTime.m_ActPointSec/1)-1;
			OneTime.m_ActPointSec=fmod(OneTime.m_ActPointSec,1)+1;

			if (OneTime.m_ActPointSec == 1)
			{
				OneTime.m_ActPointSec = 0;
				DSeconds += 1;
			}

			OneTime.m_Sec += DSeconds;
			if(OneTime.m_Sec<0)
			{
				int DMinutes=OneTime.m_Sec/60;
				OneTime.m_Sec=OneTime.m_Sec%60+60;
				OneTime.m_Min+=DMinutes-1;
				if(OneTime.m_Min<0)
				{
					int DHours=OneTime.m_Min/60;
					OneTime.m_Min=OneTime.m_Min%60+60;
					OneTime.m_Hour+=DHours-1;
					if(OneTime.m_Hour<0)
					{
						int DDays=OneTime.m_Hour/60;
						OneTime.m_Hour=OneTime.m_Hour%60+60;
						OneTime.m_Day+=DDays-1;
					}
				}
			}
	
			return OneTime;
		}
		else
		{
			return OneTime;
		}
	}
	*/

	static void GetTimeStr(GPSTime Time, char *str, int strLength)
	{
		sprintf(str, "%02d%02d%02d%02d%02d%02d.%07d",
				Time.m_Year,
				Time.m_Month,
				Time.m_Day,
				Time.m_Hour,
				Time.m_Min,
				Time.m_Sec,
				int(Time.m_ActPointSec * 10000000));
	}

	static string GetTimeStr(GPSTime Time)
	{
		char str[24];
		sprintf(str, "%02d%02d%02d%02d%02d%02d.%07d",
				Time.m_Year,
				Time.m_Month,
				Time.m_Day,
				Time.m_Hour,
				Time.m_Min,
				Time.m_Sec,
				int(Time.m_ActPointSec * 10000000));
		string str1 = str;
		return str1;
	}

	~CGPSTimeAlgorithm(void);

	static double GetPosixTime(GPSTime OneTime)
	{
		struct tm tm;
		memset(&tm, 0, sizeof(tm));
		tm.tm_year = OneTime.m_Year + 2000 - 1900;
		tm.tm_mon = OneTime.m_Month - 1;
		tm.tm_mday = OneTime.m_Day;
		tm.tm_hour = OneTime.m_Hour;
		tm.tm_min = OneTime.m_Min;
		tm.tm_sec = OneTime.m_Sec;
		time_t ft = mktime(&tm);

		double PosixTime = ft + OneTime.m_ActPointSec;
		return PosixTime;
	}

	static GPSTime GPSTimeAddingSeconds(GPSTime OneTime, double Seconds)
	{
		time_t PosixTime = (int)(GetPosixTime(OneTime)) + Seconds;
		struct tm *p = localtime(&PosixTime);
		OneTime.m_Year = p->tm_year + 1900 - 2000;
		OneTime.m_Month = p->tm_mon + 1;
		OneTime.m_Day = p->tm_mday;
		OneTime.m_Hour = p->tm_hour;
		OneTime.m_Min = p->tm_min;
		OneTime.m_Sec = p->tm_sec;

		return OneTime;
	}

	static double GetPosixTime(double YY, double MM, double DD, double hh, double mm, double ss, double Acc)
	{
		struct tm tm;
		memset(&tm, 0, sizeof(tm));
		tm.tm_year = YY - 1900;
		tm.tm_mon = MM - 1;
		tm.tm_mday = DD;
		tm.tm_hour = hh;
		tm.tm_min = mm;
		tm.tm_sec = ss;
		time_t ft = mktime(&tm);

		double PosixTime = ft + Acc;
		return PosixTime;
	}

	// static double GetPosixTimeFromHour(double YY, double MM, double DD, double hh, double mm, double ss, double Acc)
	// {
	// 	struct tm tm;
	// 	memset(&tm, 0, sizeof(tm));
	// 	tm.tm_year = YY - 1900;
	// 	tm.tm_mon = MM - 1;
	// 	tm.tm_mday = DD;
	// 	tm.tm_hour = hh;
	// 	tm.tm_min = mm;
	// 	tm.tm_sec = ss;
	// 	time_t ft = mktime(&tm);

	// 	double PosixTime = ft + Acc;
	// 	return PosixTime;
	// }

	static GPSTime GetCurrentSystemTime() //If the Point data of GPS >1, the time should be carried;
	{
		time_t seconds = time(NULL);
		struct tm *p = localtime(&seconds);
		GPSTime OneTime = GPSTime(0, 0, 0, 0, 0, 0, 0, 1, 1);
		OneTime.m_Year = p->tm_year + 1900 - 2000;
		OneTime.m_Month = p->tm_mon + 1;
		OneTime.m_Day = p->tm_mday;
		OneTime.m_Hour = p->tm_hour;
		OneTime.m_Min = p->tm_min;
		OneTime.m_Sec = p->tm_sec;
		return OneTime;
	}

	static GPSTime GPSTimeCarrtOneSecond(GPSTime OneTime) //If the Point data of GPS >1, the time should be carried;
	{
		struct tm tm;
		memset(&tm, 0, sizeof(tm));
		tm.tm_year = OneTime.m_Year + 2000 - 1900;
		tm.tm_mon = OneTime.m_Month - 1;
		tm.tm_mday = OneTime.m_Day;
		tm.tm_hour = OneTime.m_Hour;
		tm.tm_min = OneTime.m_Min;
		tm.tm_sec = OneTime.m_Sec;
		time_t ft = mktime(&tm);
		// printf("acc is %f/n", OneTime.m_ActPointSec);

		if (OneTime.m_ActPointSec >= 1)
		{
			int DSeconds = (int)(OneTime.m_ActPointSec / 1);
			OneTime.m_ActPointSec = fmod(OneTime.m_ActPointSec, 1);

			ft += DSeconds;
			struct tm *p = localtime(&ft);
			OneTime.m_Year = p->tm_year + 1900 - 2000;
			OneTime.m_Month = p->tm_mon + 1;
			OneTime.m_Day = p->tm_mday;
			OneTime.m_Hour = p->tm_hour;
			OneTime.m_Min = p->tm_min;
			OneTime.m_Sec = p->tm_sec;
		// printf("acc is %f/n", OneTime.m_ActPointSec);

		return OneTime;
		}
		else if (OneTime.m_ActPointSec < 0)
		{
			int DSeconds = (int)(OneTime.m_ActPointSec / 1) - 1;
			OneTime.m_ActPointSec = fmod(OneTime.m_ActPointSec, 1) + 1;

			if (OneTime.m_ActPointSec == 1)
			{
				OneTime.m_ActPointSec = 0;
				DSeconds += 1;
			}

			ft += DSeconds;
			struct tm *p = localtime(&ft);
			OneTime.m_Year = p->tm_year + 1900 - 2000;
			OneTime.m_Month = p->tm_mon + 1;
			OneTime.m_Day = p->tm_mday;
			OneTime.m_Hour = p->tm_hour;
			OneTime.m_Min = p->tm_min;
			OneTime.m_Sec = p->tm_sec;
		// printf("acc is %f/n", OneTime.m_ActPointSec);

			return OneTime;
		}
		else
		{
			return OneTime;
		}
	}

	static double GetGPSTimeFromHour(GPSTime PointTime)
	{
		char *time = new char[256];
		CGPSTimeAlgorithm::GetTimeStr(PointTime, time, 256);
		std::string s(&time[6], &time[20]);
		delete[] time;
		//_CrtDumpMemoryLeaks();
		return atof(s.c_str()); //日以后转换为double类型的时间
	}
};
