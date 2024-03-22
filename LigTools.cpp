#include "LigTools.h"

#include <filesystem>

LigTools::LigTools() {}

LigTools::~LigTools() {}

string LigTools::GetLocFileName(string filePath, const GPSTime Time) {
  string FileName = filePath;
  if (!filesystem::exists(FileName)) {
    filesystem::create_directory(FileName);
  }
  FileName += "/";
  FileName += CGPSTimeAlgorithm::GetTimeString(Time).substr(0, 6);
  if (!filesystem::exists(FileName)) {
    filesystem::create_directory(FileName);
  }
  FileName += "/";
  FileName += CGPSTimeAlgorithm::GetTimeString(Time).substr(0, 8);
  if (!filesystem::exists(FileName)) {
    filesystem::create_directory(FileName);
  }
  FileName += "/";
  FileName += CGPSTimeAlgorithm::GetTimeString(Time).substr(0, 10);
  FileName += ".loc";
  return FileName;
}

bool LigTools::check_location_structure(const vector<LocSta> &Stations, LocSta &result,
                                        double checkTheta) {
  bool valid{true};
  Cartesian O = Cartesian(result.Lat, result.Lon);
  Cartesian P1 = Cartesian(result.Lat, result.Lon);
  Cartesian P2 = Cartesian(result.Lat, result.Lon);
  double u1{0.0}, u2{0.0}, u3{0.0}, v1{0.0}, v2{0.0}, v3{0.0}, ulength{0.0}, vlength{0.0},
      dotproduct{0.0}, theta{0.0}, max_theta{0.0};
  for (int i = 0; i < Stations.size() - 1; i++) {
    for (int j = i + 1; j < Stations.size(); j++) {
      P1 = Cartesian(Stations[i].Lat, Stations[i].Lon);
      P2 = Cartesian(Stations[j].Lat, Stations[j].Lon);
      u1 = O.x - P1.x;
      u2 = O.y - P1.y;
      u3 = O.z - P1.z;
      v1 = O.x - P2.x;
      v2 = O.y - P2.y;
      v3 = O.z - P2.z;
      ulength = sqrt(u1 * u1 + u2 * u2 + u3 * u3);
      vlength = sqrt(v1 * v1 + v2 * v2 + v3 * v3);
      dotproduct = u1 * v1 + u2 * v2 + u3 * v3;
      theta = acos(dotproduct / (ulength * vlength)) * radians2degree;
      max_theta = max(max_theta, theta);
    }
  }
  if (max_theta < checkTheta) {
    result.sq = FLOAT_MAX;
    valid = false;
  }
  return valid;
}

vector<vector<TriggerInfo>> LigTools::getLocationPool(
    ordered_map<int, triggerAtStation> &triggerPool) {
  vector<vector<TriggerInfo>> locCombinationPool;
  int m = 0;
  for (auto &oneStation : triggerPool) {
    size_t nPool = locCombinationPool.size();
    size_t cachePoolLimit = oneStation.second.triggers.size();

    if (m) {
      if (nPool == 0) break;
      for (int k = cachePoolLimit - 1; k > -1; --k) {
        if (k) {
          for (int p = 0; p < nPool; ++p) {
            locCombinationPool.push_back(locCombinationPool[p]);
            locCombinationPool.back().push_back(oneStation.second.triggers[k]);
          }
        } else {
          for (int p = nPool - 1; p > -1; --p) {
            locCombinationPool[p].push_back(oneStation.second.triggers[k]);
          }
        }
      }
    } else {
      for (int k = cachePoolLimit - 1; k > -1; --k) {
        vector<TriggerInfo> oneComb;
        oneComb.push_back(oneStation.second.triggers[k]);
        locCombinationPool.push_back(oneComb);
      }
    }
    m++;
  }

  return locCombinationPool;
}

void LigTools::permuteVector(vector<vector<TriggerInfo>> triggers, vector<TriggerInfo> &current,
                             int index, vector<vector<TriggerInfo>> &CombinationPool,
                             unordered_map<int, unordered_map<int, double>> &siteTimeMap) {
  if (index == triggers.size()) {
    // 已经枚举到了最后一个数组，将其添加到最终结果，当前排列完成
    CombinationPool.emplace_back(current);
    return;
  }

  for (auto i = 0; i < triggers[index].size(); i++) {
    bool valid{true};
    auto &tri{triggers[index][i]};
    for (const auto &element : current) {
      // add 0.0001
      if (abs(tri.time - element.time) > siteTimeMap[element.stationID][tri.stationID] + 0.0001) {
        valid = false;
        break;
      }
      // if (tri.Value * element.Value < 0) {
      //	valid = false;
      //	break;
      // }
    }
    if (valid) {
      current.emplace_back(tri);
      permuteVector(triggers, current, index + 1, CombinationPool, siteTimeMap);
      // 执行到这里，说明已经成功返回了一组组合，所以pop_back(）一个元素以便进行新的排列
      current.pop_back();
    }
  }
}

// vector<vector<TriggerInfo>> LigTools::getLocationPool_p(map<int, triggerAtStation> &triggerPool, map<int, map<int, double>> &siteTimeMap)
// {
// 	vector<vector<TriggerInfo>> locCombinationPool;
// 	vector<vector<TriggerInfo>> triggers;
// 	vector<TriggerInfo> current;
// 	for (auto &iter : triggerPool)
// 	{
// 		triggers.push_back(iter.second.triggers);
// 	}
// 	permuteVector(triggers, current, 0, locCombinationPool, siteTimeMap);
// 	return locCombinationPool;
// }

void generateCombinations(vector<int> &nums, int k, int start, vector<int> &current,
                          vector<vector<int>> &combinations) {
  if (current.size() == k) {
    combinations.emplace_back(current);
    return;
  }

  for (int i = start; i < nums.size(); i++) {
    current.push_back(nums[i]);
    generateCombinations(nums, k, i + 1, current, combinations);
    current.pop_back();
  }
}

vector<vector<int>> getCombinations(vector<int> nums, int k) {
  vector<vector<int>> combinations;
  vector<int> current;
  generateCombinations(nums, k, 0, current, combinations);
  return combinations;
}

vector<vector<TriggerInfo>> LigTools::getLocationPool_p(
    ordered_map<int, triggerAtStation> &triggerPool,
    unordered_map<int, unordered_map<int, double>> &siteTimeMap, int nCombStas) {
  vector<vector<TriggerInfo>> locCombinationPool;
  vector<vector<TriggerInfo>> triggers;
  vector<TriggerInfo> current;
  for (auto &iter : triggerPool) {
    triggers.emplace_back(iter.second.triggers);
  }

  // 获取n个站点中选取任意5个站点的组合
  vector<int> stationIndices;
  for (int i = 0; i < triggers.size(); i++) {
    stationIndices.emplace_back(i);
  }
  vector<vector<int>> combinations = getCombinations(stationIndices, nCombStas);

  for (const auto &combination : combinations) {
    vector<vector<TriggerInfo>> selectedTriggers;
    for (int i : combination) {
      selectedTriggers.emplace_back(triggers[i]);
    }
    permuteVector(selectedTriggers, current, 0, locCombinationPool, siteTimeMap);
  }

  return locCombinationPool;
}