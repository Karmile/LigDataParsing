
#include "LocStruct.h"

#include <Eigen/Core>

#include "Params.h"
#include "ceres/ceres.h"
#include "glog/logging.h"
#include "yaml-cpp/yaml.h"

std::string configFile = (".\\config.yaml");
YAML::Node config = YAML::LoadFile(configFile);

LocSta FinalGeoLocation_GPU(vector<LocSta> Stations, vector<double> Loc_Time, LocSta result) {
  int NumOfSta = Stations.size();
  LocSta* pLocSta = new LocSta[NumOfSta];
  memcpy(pLocSta, &Stations[0], sizeof(LocSta) * NumOfSta);
  DATAPREC* pDiffBe2s = new DATAPREC[NumOfSta];
  LocSta resultFinal;

  for (int m = 1; m != Stations.size(); ++m) {
    pDiffBe2s[m] = (Loc_Time[m] - Loc_Time[m - 1]) * cVeo;
  }
  pDiffBe2s[0] = (Loc_Time[0] - Loc_Time[Loc_Time.size() - 1]) * cVeo;

  //for (int i = 0; i != Stations.size(); ++i)
  //{
  //	pDiffBe2s[i] = (Loc_Time[i] - Loc_Time[NumOfSta-1]) * cVeo;
  //}

  LocPara locPara_S;

  locPara_S.Lat = 0.0005 * PI / 180;
  locPara_S.boundW = (result.Lon - 0.016) * PI / 180;
  locPara_S.boundS = (result.Lat - 0.016) * PI / 180;
  locPara_S.boundE = (result.Lon + 0.0162001) * PI / 180;
  locPara_S.boundN = (result.Lat + 0.0162001) * PI / 180;
  locPara_S.boundht = result.h + 1.6;
  locPara_S.boundhb = result.h - 1.6;
  if (locPara_S.boundhb < 0) locPara_S.boundhb = 0;
  if (locPara_S.boundht > 20) locPara_S.boundht = 20;

  locPara_S.dh = 0.05;
  LocCuda New_loccuda_Final;
  New_loccuda_Final.GetLocPara(locPara_S, pLocSta, NumOfSta);
  //resultFinal = New_loccuda_Final.Location3D_GPU(pDiffBe2s, NumOfSta);
  //选取Loc_Time里最小的值的索引
  int minIndex = 0;
  for (int i = 1; i < Loc_Time.size(); i++) {
    if (Loc_Time[i] < Loc_Time[minIndex]) minIndex = i;
  }
  resultFinal.occur_t = Loc_Time[minIndex] - Stadistance(Stations[minIndex].Lat * radians2degree,
                                                         Stations[minIndex].Lon * radians2degree,
                                                         resultFinal.Lat, resultFinal.Lon) /
                                                 cVeo;

  delete[] pDiffBe2s;
  delete[] pLocSta;
  return resultFinal;
}

LocSta GeoLocation_GPU_Initial(vector<LocSta> Stations, vector<double> Loc_Time) {
  LocPara locPara = LocPara{0.04 * PI / 180,
                            boundaryW * PI / 180,
                            boundaryE * PI / 180,
                            boundaryS * PI / 180,
                            boundaryN * PI / 180,
                            0,
                            20,
                            5};
  int NumOfSta = Stations.size();
  LocSta* pLocSta = new LocSta[NumOfSta];
  memcpy(pLocSta, &Stations[0], sizeof(LocSta) * NumOfSta);
  LocCuda loccuda;
  loccuda.GetLocPara(locPara, pLocSta, NumOfSta);
  DATAPREC* pDiffBe2s = new DATAPREC[NumOfSta];
  LocSta result;

  for (int m = 1; m != Stations.size(); ++m) {
    pDiffBe2s[m] = (Loc_Time[m] - Loc_Time[m - 1]) * cVeo;
  }
  pDiffBe2s[0] = (Loc_Time[0] - Loc_Time[Loc_Time.size() - 1]) * cVeo;

  //for (int i = 0; i != Stations.size(); ++i)
  //{
  //	pDiffBe2s[i] = (Loc_Time[i] - Loc_Time[NumOfSta - 1]) * cVeo;
  //}

  result = loccuda.Location3D_GPU(pDiffBe2s, NumOfSta);

  if (result.sq < 30) {
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

    locPara_S.Lat = 0.005 * PI / 180;
    locPara_S.boundW = (result.Lon - 0.640) * PI / 180;
    locPara_S.boundS = (result.Lat - 0.640) * PI / 180;
    locPara_S.boundE = (result.Lon + 0.640001) * PI / 180;
    locPara_S.boundN = (result.Lat + 0.640001) * PI / 180;
    locPara_S.boundht = result.h + 6;
    locPara_S.boundhb = result.h - 6;
    if (locPara_S.boundhb < 0) locPara_S.boundhb = 0;
    if (locPara_S.boundht > 20) locPara_S.boundht = 20;

    locPara_S.dh = 0.5;
    LocCuda New_loccuda_Final;
    New_loccuda_Final.GetLocPara(locPara_S, pLocSta, NumOfSta);
    result = New_loccuda_Final.Location3D_GPU(pDiffBe2s, NumOfSta);
  }

  int minIndex = 0;
  for (int i = 1; i < Loc_Time.size(); i++) {
    if (Loc_Time[i] < Loc_Time[minIndex]) minIndex = i;
  }
  result.occur_t = Loc_Time[minIndex] - Stadistance(Stations[minIndex].Lat * radians2degree,
                                                    Stations[minIndex].Lon * radians2degree,
                                                    result.Lat, result.Lon) /
                                            cVeo;

  //check_location_structure(Stations, result);
  delete[] pDiffBe2s;
  delete[] pLocSta;
  return result;
}

LocSta GeoLocation_GPU(vector<LocSta> Stations, vector<double> Loc_Time) {
  LocPara locPara = LocPara{0.08 * PI / 180,
                            boundaryW * PI / 180,
                            boundaryE * PI / 180,
                            boundaryS * PI / 180,
                            boundaryN * PI / 180,
                            0,
                            0.1,
                            0.1};

  int NumOfSta = Stations.size();
  LocSta* pLocSta = new LocSta[NumOfSta];
  memcpy(pLocSta, &Stations[0], sizeof(LocSta) * NumOfSta);
  LocCuda loccuda;
  loccuda.GetLocPara(locPara, pLocSta, NumOfSta);
  DATAPREC* pDiffBe2s = new DATAPREC[NumOfSta];
  LocSta result;

  for (int m = 1; m != Stations.size(); ++m) {
    pDiffBe2s[m] = (Loc_Time[m] - Loc_Time[m - 1]) * cVeo;
  }
  pDiffBe2s[0] = (Loc_Time[0] - Loc_Time[Loc_Time.size() - 1]) * cVeo;

  //for (int i = 0; i != Stations.size(); ++i)
  //{
  //	pDiffBe2s[i] = (Loc_Time[i] - Loc_Time[NumOfSta - 1]) * cVeo;
  //}

  result = loccuda.Location3D_GPU(pDiffBe2s, NumOfSta);

  if (result.sq < 50) {
    LocPara locPara_S;

    locPara_S.Lat = 0.01 * PI / 180;
    locPara_S.boundW = (result.Lon - 1.28) * PI / 180;
    locPara_S.boundS = (result.Lat - 1.28) * PI / 180;
    locPara_S.boundE = (result.Lon + 1.28001) * PI / 180;
    locPara_S.boundN = (result.Lat + 1.28001) * PI / 180;
    locPara_S.boundht = 20;
    locPara_S.boundhb = 0;
    locPara_S.dh = 1;

    LocCuda New_loccuda;
    New_loccuda.GetLocPara(locPara_S, pLocSta, NumOfSta);
    result = New_loccuda.Location3D_GPU(pDiffBe2s, NumOfSta);

    locPara_S.Lat = 0.001 * PI / 180;
    locPara_S.boundW = (result.Lon - 0.064) * PI / 180;
    locPara_S.boundS = (result.Lat - 0.064) * PI / 180;
    locPara_S.boundE = (result.Lon + 0.064001) * PI / 180;
    locPara_S.boundN = (result.Lat + 0.064001) * PI / 180;
    locPara_S.boundht = result.h + 3.2;
    locPara_S.boundhb = result.h - 3.2;
    if (locPara_S.boundhb < 0) locPara_S.boundhb = 0;
    if (locPara_S.boundhb > 20) locPara_S.boundht = 20;

    locPara_S.dh = 0.1;
    LocCuda New_loccuda_Final;
    New_loccuda_Final.GetLocPara(locPara_S, pLocSta, NumOfSta);
    result = New_loccuda_Final.Location3D_GPU(pDiffBe2s, NumOfSta);
  }

  int minIndex = 0;
  for (int i = 1; i < Loc_Time.size(); i++) {
    if (Loc_Time[i] < Loc_Time[minIndex]) minIndex = i;
  }
  result.occur_t = Loc_Time[minIndex] - Stadistance(Stations[minIndex].Lat * radians2degree,
                                                    Stations[minIndex].Lon * radians2degree,
                                                    result.Lat, result.Lon) /
                                            cVeo;

  //result.occur_t = 0;
  //for (int i = 0; i < Loc_Time.size(); i++)
  //{
  //	result.occur_t = result.occur_t + Loc_Time[i] - Stadistance(Stations[i].Lat * radians2degree, Stations[i].Lon * radians2degree, result.Lat, result.Lon) / cVeo;
  //}
  //result.occur_t = result.occur_t / (Loc_Time.size());

  //check_location_structure(Stations, result);
  delete[] pDiffBe2s;
  delete[] pLocSta;
  return result;
}
// add optimise solution

struct CostFunctor {
  const std::vector<double>& t_;
  const std::vector<LocSta>& stations_;

  CostFunctor(const std::vector<double>& t, const vector<LocSta>& stations)
      : t_(t), stations_(stations) {}
  template <typename T>
  bool operator()(const T* const params, T* residual) const {
    for (int i = 0; i < stations_.size(); i++) {
      auto& st = stations_[i];
      T dlon = T(st.Lon) - params[1];
      T dlat = T(st.Lat) - params[0];
      T a = sin(dlat / T(2)) * sin(dlat / T(2)) +
            cos(params[0]) * cos(T(st.Lat)) * sin(dlon / T(2)) * sin(dlon / T(2));
      T c = T(2) * atan2(sqrt(a), sqrt(T(1) - a));
      T hdist = c * R;
      T dist = sqrt(pow((T(st.h) - params[2]), 2) + pow(hdist, 2));
      T tt = dist / cVeo;
      T diff = T(t_[i]) - tt - params[3];
      residual[i] = diff;
    }

    return true;
  }
};

LocSta GeoLocation_OP(vector<LocSta> Stations, vector<double> Loc_Time, LocSta initResult,
                      int num_threads) {
  // ���� Ceres ����
  ceres::Problem problem;

  // ���Ӳ���
  double params[4] = {30 * degree2radians, 115 * degree2radians, 1.0, Loc_Time.back()};  // ��ʼ����
  //判断result输入是否存在
  if (initResult.Lat != 0 && initResult.Lon != 0 && initResult.h != 0) {
    params[0] = initResult.Lat * degree2radians;
    params[1] = initResult.Lon * degree2radians;
    params[2] = initResult.h;
    params[3] = Loc_Time.back();
  }

  // ���Ӳв���
  ceres::CostFunction* cost_function =
      new ceres::AutoDiffCostFunction<CostFunctor, ceres::DYNAMIC, 4>(
          new CostFunctor(Loc_Time, Stations), Stations.size());
  problem.AddResidualBlock(cost_function, nullptr, params);
  // �������ѡ��
  Bounds para = Bounds(90.0, 140.0, 0.0, 60.0, 0, 20.0);
  problem.SetParameterLowerBound(params, 0, para.boundS);
  problem.SetParameterUpperBound(params, 0, para.boundN);
  problem.SetParameterLowerBound(params, 1, para.boundW);
  problem.SetParameterUpperBound(params, 1, para.boundE);
  problem.SetParameterLowerBound(params, 2, para.boundhb);
  problem.SetParameterUpperBound(params, 2, para.boundht);
  ceres::Solver::Options options;
  // 设置使用LM算法求解

  //options.num_threads = omp_get_max_threads();

  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = false;
  options.num_threads = num_threads;  //omp_get_max_threads();

  // ���
  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);
  //������
  //std::cout << summary.BriefReport() << std::endl;
  //std::cout << "Estimated parameters: ";
  LocSta result(params[0], params[1], params[2], params[3]);
  result.sq = sqrt(summary.final_cost / Stations.size()) * cVeo;
  result.Lat *= radians2degree;
  result.Lon *= radians2degree;
  return result;
}

struct CostFunctor_2 {
  const std::vector<double>& t_;
  const std::vector<LocSta>& stations_;

  CostFunctor_2(const std::vector<double>& t, const vector<LocSta>& stations)
      : t_(t), stations_(stations) {}
  template <typename T>
  bool operator()(const T* const params, T* residual) const {
    T dis1, dis2;

    auto& st = stations_[0];
    T dlon = T(st.Lon) - params[1];
    T dlat = T(st.Lat) - params[0];

    //T a = sin(dlat / T(2)) * sin(dlat / T(2)) +
    //      cos(params[0]) * cos(T(st.Lat)) * sin(dlon / T(2)) * sin(dlon / T(2));
    //T c = T(2) * atan2(sqrt(a), sqrt(T(1) - a));
    //T hdist = c * R;

    T c1 = cos(params[0]);
    T c2 = cos(T(st.Lat));
    T d1 = sin(dlat / 2.0);
    T d2 = sin(dlon / 2.0);
    T hdist = 2.0 * T(R) * asin(sqrt(d1 * d1 + c1 * c2 * d2 * d2));

    dis1 = sqrt(pow((T(st.h) - params[2]), 2) + pow(hdist, 2));

    T disSta9 = dis1;

    for (int i = 1; i < stations_.size(); i++) {
      dis2 = dis1;

      auto& sti = stations_[i];
      dlon = T(sti.Lon) - params[1];
      dlat = T(sti.Lat) - params[0];
      //a = sin(dlat / T(2)) * sin(dlat / T(2)) +
      //    cos(params[0]) * cos(T(sti.Lat)) * sin(dlon / T(2)) * sin(dlon / T(2));
      //c = T(2) * atan2(sqrt(a), sqrt(T(1) - a));
      //hdist = c * R;

      c1 = cos(params[0]);
      c2 = cos(T(sti.Lat));
      d1 = sin(dlat / 2.0);
      d2 = sin(dlon / 2.0);
      hdist = 2.0 * T(R) * asin(sqrt(d1 * d1 + c1 * c2 * d2 * d2));

      dis1 = sqrt(pow((T(sti.h) - params[2]), 2) + pow(hdist, 2));
      residual[i] = dis1 - dis2 - (T(t_[i]) - T(t_[i - 1])) * cVeo;
    }

    residual[0] = disSta9 - dis1 - (T(t_[0]) - T(t_[stations_.size() - 1])) * cVeo;

    return true;
  }
};
#define GOOGLE_STRIP_LOG 1

LocSta GeoLocation_OP_2(vector<LocSta> Stations, vector<double> Loc_Time, LocSta initResult,
                        int num_threads) {
  // ���� Ceres ����
  ceres::Problem problem;

  // ���Ӳ���
  double params[3] = {30 * degree2radians, 115 * degree2radians, 1.0};  // ��ʼ����
  //判断result输入是否存在
  if (initResult.Lat != 0 && initResult.Lon != 0 && initResult.h != 0) {
    params[0] = initResult.Lat * degree2radians;
    params[1] = initResult.Lon * degree2radians;
    params[2] = initResult.h;
  }

  // ���Ӳв���
  ceres::CostFunction* cost_function =
      new ceres::AutoDiffCostFunction<CostFunctor_2, ceres::DYNAMIC, 3>(
          new CostFunctor_2(Loc_Time, Stations), Stations.size());
  problem.AddResidualBlock(cost_function, nullptr, params);
  // �������ѡ��
  Bounds para = Bounds(40.0, 190.0, -60.0, 80.0, 0, 20.0);
  problem.SetParameterLowerBound(params, 0, para.boundS);
  problem.SetParameterUpperBound(params, 0, para.boundN);
  problem.SetParameterLowerBound(params, 1, para.boundW);
  problem.SetParameterUpperBound(params, 1, para.boundE);
  problem.SetParameterLowerBound(params, 2, para.boundhb);
  problem.SetParameterUpperBound(params, 2, para.boundht);
  ceres::Solver::Options options;
  // 设置使用LM算法求解

  //options.num_threads = omp_get_max_threads();

  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = false;
  options.num_threads = num_threads;  //omp_get_max_threads();
  options.logging_type = ceres::SILENT;

  // ���
  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);
  //������
  //std::cout << summary.BriefReport() << std::endl;
  //std::cout << "Estimated parameters: ";
  LocSta result;
  result.sq = sqrt(summary.final_cost / Stations.size());
  result.Lat = params[0] * radians2degree;
  result.Lon = params[1] * radians2degree;
  result.h = params[2];

  int minIndex = 0;
  for (int i = 1; i < Loc_Time.size(); i++) {
    if (Loc_Time[i] < Loc_Time[minIndex]) minIndex = i;
  }
  result.occur_t = Loc_Time[minIndex] - Stadistance(Stations[minIndex].Lat * radians2degree,
                                                    Stations[minIndex].Lon * radians2degree,
                                                    result.Lat, result.Lon) /
                                            cVeo;

  return result;
}
