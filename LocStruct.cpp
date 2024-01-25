
#include "LocStruct.h"
#include "Params.h"
#include "ceres/ceres.h"
#include "glog/logging.h"
#include <Eigen/Core>
LocSta FinalGeoLocation(vector<LocSta> Stations, vector<double> Loc_Time, LocSta result)
{
	int NumOfSta = Stations.size();
	LocSta *pLocSta = new LocSta[NumOfSta];
	memcpy(pLocSta, &Stations[0], sizeof(LocSta) * NumOfSta);
	DATAPREC *pDiffBe2s = new DATAPREC[NumOfSta];
	LocSta resultFinal;

	for (int m = 1; m != Stations.size(); ++m)
	{
		pDiffBe2s[m] = (Loc_Time[m] - Loc_Time[m - 1]) * cVeo*0.99;
	}
	pDiffBe2s[0] = (Loc_Time[0] - Loc_Time[Loc_Time.size() - 1]) * cVeo * 0.99;

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
	locPara.Lat = 0.04 * PI / 180;
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
		pDiffBe2s[m] = (Loc_Time[m] - Loc_Time[m - 1]) * cVeo * 0.99;
	}
	pDiffBe2s[0] = (Loc_Time[0] - Loc_Time[Loc_Time.size() - 1]) * cVeo * 0.99;

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

		locPara_S.dh = 0.5;
		LocCuda New_loccuda_Final;
		New_loccuda_Final.GetLocPara(locPara_S, pLocSta, NumOfSta);
		result = New_loccuda_Final.Location3D_GPU(pDiffBe2s, NumOfSta);
	}

	delete[] pDiffBe2s;
	delete[] pLocSta;
	return result;
}
// add optimise solution 

struct DynamicCostFunctor {
	const std::vector<double>& t;
	const vector<LocSta>& stations_;

	DynamicCostFunctor(const std::vector<double>& t, const vector<LocSta>& stations)
		: t(t), stations_(stations) {}
	template <typename T>
	bool operator()(const T* const params, T* residual) const {
		for (size_t i = 0; i < stations_.size(); i++) {
			auto& st = stations_[i];
			T hdist_ = sqrt(pow((T(st.Lon) - params[0]), 2) + pow((T(st.Lat) - params[1]), 2));
			T hdist = hdist_ * R;
			T dist = sqrt(pow((T(st.h) - params[2]), 2) + pow(hdist, 2));
			T tt = dist / cVeo;
			T t_diff = T(t[i]) - tt - params[3];
			residual[i] = t_diff;
		}

		return true;
	}
};
LocSta GeoLocation_OP(vector<LocSta> Stations, vector<double> Loc_Time) {

	// 构建 Ceres 问题
	ceres::Problem problem;

	// 添加参数
	double params[4] = { 5.0, 5.00, 10.0, Loc_Time.back() }; // 初始参数
	problem.AddParameterBlock(params, 4);

	//// 添加残差项
	//ceres::DynamicAutoDiffCostFunction<DynamicCostFunctor>* cost_function =
	//	new ceres::DynamicAutoDiffCostFunction<DynamicCostFunctor>(
	//		new DynamicCostFunctor(Loc_Time, Stations));
	//cost_function->SetNumResiduals(Stations.size());
	//// 实例化模板函数为特定类型
	//ceres::ResidualBlockId residual_block_id;
	//ceres::internal::VariadicAutoDiff<DynamicAutoDiffCostFunction, double,
	//	ceres::DYNAMIC, DynamicAutoDiffCostFunction::kNumResiduals,
	//	ceres::DYNAMIC, DynamicAutoDiffCostFunction::kParameterBlockSize>
	//	::Differentiate(cost_function, params, &residual_block_id);
	//problem.AddResidualBlock(cost_function, nullptr, params);

	//// 配置求解选项
	//ceres::Solver::Options options;
	//options.linear_solver_type = ceres::DENSE_SCHUR;
	//options.minimizer_progress_to_stdout = true;

	//// 求解
	//ceres::Solver::Summary summary;
	//ceres::Solve(options, &problem, &summary);

	// 输出结果
	//std::cout << summary.BriefReport() << std::endl;
	std::cout << "Estimated parameters: ";
	for (int i = 0; i < 4; ++i) {
		if (i < 2)
		{
			params[i] *= radians2degree;
		}
		std::cout << params[i] << " ";
	}
	std::cout << std::endl;
	LocSta result(params[0], params[1], params[2]);

	return result;
}
