
#include "LocStruct.h"
#include "Params.h"

LocSta FinalGeoLocation(vector<LocSta> Stations, vector<double> Loc_Time, LocSta result)
{
	int NumOfSta = Stations.size();
	LocSta *pLocSta = new LocSta[NumOfSta];
	memcpy(pLocSta, &Stations[0], sizeof(LocSta) * NumOfSta);
	DATAPREC *pDiffBe2s = new DATAPREC[NumOfSta];
	LocSta resultFinal;

	for (int m = 1; m != Stations.size(); ++m)
	{
		pDiffBe2s[m] = (Loc_Time[m] - Loc_Time[m - 1]) * cVeo;
	}
	pDiffBe2s[0] = (Loc_Time[0] - Loc_Time[Loc_Time.size() - 1]) * cVeo;

	//for (int i = 0; i != Stations.size(); ++i)
	//{
	//	pDiffBe2s[i] = (Loc_Time[i] - Loc_Time[NumOfSta-1]) * cVeo;
	//}

	LocPara locPara_S;

	locPara_S.Lat = 0.00005 * PI / 180;
	locPara_S.boundW = (result.Lon - 0.016) * PI / 180;
	locPara_S.boundS = (result.Lat - 0.016) * PI / 180;
	locPara_S.boundE = (result.Lon + 0.0162001) * PI / 180;
	locPara_S.boundN = (result.Lat + 0.0162001) * PI / 180;
	locPara_S.boundht = result.h + 1.6;
	locPara_S.boundhb = result.h - 1.6;
	if (locPara_S.boundhb < 0)
		locPara_S.boundhb = 0;
	if (locPara_S.boundht > 20)
		locPara_S.boundht = 20;

	locPara_S.dh = 0.005;
	LocCuda New_loccuda_Final;
	New_loccuda_Final.GetLocPara(locPara_S, pLocSta, NumOfSta);
	resultFinal = New_loccuda_Final.Location3D_GPU(pDiffBe2s, NumOfSta);

	delete[] pDiffBe2s;
	delete[] pLocSta;
	return resultFinal;
}

LocSta GeoLocation(vector<LocSta> Stations, vector<double> Loc_Time)
{
	LocPara locPara;
	locPara.boundE = boundaryE * PI / 180;
	locPara.boundS = boundaryS * PI / 180;
	locPara.boundW = boundaryW * PI / 180;
	locPara.boundN = boundaryN * PI / 180;
	locPara.Lat = 0.01 * PI / 180;
	locPara.boundht = 20;
	locPara.boundhb = 0;
	locPara.dh = 1;

	int NumOfSta = Stations.size();
	LocSta *pLocSta = new LocSta[NumOfSta];
	memcpy(pLocSta, &Stations[0], sizeof(LocSta) * NumOfSta);
	LocCuda loccuda;
	loccuda.GetLocPara(locPara, pLocSta, NumOfSta);
	DATAPREC *pDiffBe2s = new DATAPREC[NumOfSta];
	LocSta result;

	for (int m = 1; m != Stations.size(); ++m)
	{
		pDiffBe2s[m] = (Loc_Time[m] - Loc_Time[m - 1]) * cVeo;
	}
	pDiffBe2s[0] = (Loc_Time[0] - Loc_Time[Loc_Time.size() - 1]) * cVeo;

	//for (int i = 0; i != Stations.size(); ++i)
	//{
	//	pDiffBe2s[i] = (Loc_Time[i] - Loc_Time[NumOfSta - 1]) * cVeo;
	//}

	result = loccuda.Location3D_GPU(pDiffBe2s, NumOfSta);

	if (result.sq < 30)
	{
		LocPara locPara_S;

		//locPara_S.Lat = 0.01 * PI / 180;
		//locPara_S.boundW = (result.Lon - 0.64) * PI / 180;
		//locPara_S.boundS = (result.Lat - 0.64) * PI / 180;
		//locPara_S.boundE = (result.Lon + 0.64001) * PI / 180;
		//locPara_S.boundN = (result.Lat + 0.64001) * PI / 180;
		//locPara_S.boundht = 20;
		//locPara_S.boundhb = 0;
		//locPara_S.dh = 1;

		//LocCuda New_loccuda;
		//New_loccuda.GetLocPara(locPara_S, pLocSta, NumOfSta);
		//result = New_loccuda.Location3D_GPU(pDiffBe2s, NumOfSta);

		locPara_S.Lat = 0.001 * PI / 180;
		locPara_S.boundW = (result.Lon - 0.032) * PI / 180;
		locPara_S.boundS = (result.Lat - 0.032) * PI / 180;
		locPara_S.boundE = (result.Lon + 0.032001) * PI / 180;
		locPara_S.boundN = (result.Lat + 0.032001) * PI / 180;
		locPara_S.boundht = result.h + 3.2;
		locPara_S.boundhb = result.h - 3.2;
		if (locPara_S.boundhb < 0)
			locPara_S.boundhb = 0;
		if (locPara_S.boundht > 20)
			locPara_S.boundht = 20;

		locPara_S.dh = 0.1;
		LocCuda New_loccuda_Final;
		New_loccuda_Final.GetLocPara(locPara_S, pLocSta, NumOfSta);
		result = New_loccuda_Final.Location3D_GPU(pDiffBe2s, NumOfSta);
	}

	delete[] pDiffBe2s;
	delete[] pLocSta;
	return result;
}
