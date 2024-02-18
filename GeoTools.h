#pragma once
#include<string>
#include<cmath>
#include"Params.h"
using namespace std;

inline double rads(double x)
{
	return x * 3.1415926535897931 * 1.0 / 180;
}

inline double Stadistance(double LatA, double LonA, double LatB, double LonB)
{
	LatA = rads(LatA);
	LatB = rads(LatB);
	LonA = rads(LonA);
	LonB = rads(LonB);

	//double distance = R * acos(cos(LatA)*cos(LatB)*cos(LonB - LonA) + sin(LatA)*sin(LatB));
	//return distance;

	return  (2.0f * asin(fmin(1.0f, sqrtf(sin((LatB - LatA) / 2) * sin((LatB - LatA) / 2) + sin((LonB - LonA) / 2) * sin((LonB - LonA) / 2) * cos(LatA) * cos(LatB)))))*R;
}

struct Cartesian {
	Cartesian(double x_, double y_, double z_) :x(x_), y(y_), z(z_) {};
	Cartesian(double lan, double lon) {
		x = cos(lan) * cos(lon);
		y = cos(lan) * sin(lon);
		z = sin(lan);
	}
	double x;
	double y;
	double z;
};