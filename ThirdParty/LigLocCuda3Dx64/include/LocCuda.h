#pragma once
#include <stdio.h>
#include <string.h>
#include "PublicPara.h"

class LocCuda
{
public:
	LocCuda()
	{
		m_pLocSta=NULL;
		pLutCuda=NULL;
	}

	~LocCuda()
	{		
		if(m_pLocSta)
			delete[] m_pLocSta;
	}

	void BuildLUT();


	LocSta Location3D(DATAPREC* pDiffBe2s, int NumOfDiff);
	LocSta Location2D(DATAPREC* pDiffBe2s, int NumOfDiff);
	LocSta Location3D_GPU(DATAPREC* pDiffBe2s, int NumOfDiff);
	LocSta LocationIF_GPU(DATAPREC* pDiffBe2s, int NumOfDiff);

	LocSta LocLUT(DATAPREC* pDiffBe2s, int NumOfDiff);
	void GetLocPara(LocPara LocPara,LocSta* pLocSta,int NumOfSta)
	{
		m_LocPara=LocPara;
		m_NumOfSta=NumOfSta;

		if(m_pLocSta)
			delete[] m_pLocSta;

		m_pLocSta=new LocSta[NumOfSta];
		memcpy(m_pLocSta,pLocSta,sizeof(LocSta)*NumOfSta);
	}

	void FreeCache();


	LocSta Location2D_CPU(DATAPREC* pDiffBe2s, int NumOfDiff);
	LocSta Location2D_CPU_LibBuild(DATAPREC* pDiffBe2s, int NumOfDiff);
	LocSta Location2D_CPU_Lib(DATAPREC* pDiffBe2s, int NumOfDiff);
	double** Location2D_CPULocationLib;

	DATAPREC distance(DATAPREC LatA, DATAPREC LonA, DATAPREC LatB, DATAPREC LonB);


private:
	LocSta* m_pLocSta;
	int m_NumOfSta;
	LocPara m_LocPara;
	DATAPREC* pLutCuda;

};