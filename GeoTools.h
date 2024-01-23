#pragma once
#include<string>
#include<cmath>
using namespace std;

double rads(double x)
{
	return x * 3.1415926535897931 * 1.0 / 180;
}

double Stadistance(double LatA, double LonA, double LatB, double LonB)
{
	LatA = rads(LatA);
	LatB = rads(LatB);
	LonA = rads(LonA);
	LonB = rads(LonB);
	double R = 6371.004;
	//double distance = R * acos(cos(LatA)*cos(LatB)*cos(LonB - LonA) + sin(LatA)*sin(LatB));
	//return distance;

	return 6371.004 * (2.0f * asin(fmin(1.0f, sqrtf(sin((LatB - LatA) / 2) * sin((LatB - LatA) / 2) + sin((LonB - LonA) / 2) * sin((LonB - LonA) / 2) * cos(LatA) * cos(LatB)))));
}