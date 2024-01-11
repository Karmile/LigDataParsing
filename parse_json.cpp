#include <iostream>
#include <json/json.h>
#include <httplib.h>
#include <DataStruct.h>

// 定义结构体来存储站点信息


int main() {
    // 发起HTTPS请求获取站点信息
    httplib::Client client("http://112.26.166.27:18000");
    auto res = client.Get("/getDetectionNetworkStatesApi");

    if (res && res->status == 200) {
        // 解析JSON响应
        JSONCPP_STRING err;
        Json::Value root;
        Json::Reader reader;
        //Json::Reader reader;
        const auto rawJsonLength = static_cast<int>(res->body.length());
        cout << res->body.c_str();
        try {
            if (reader.parse(res->body.c_str(),res->body.c_str() + rawJsonLength, root)) {
                // 遍
                // 历JSON数组并将站点信息存储到结构体中
                if (root.empty()) {
                    std::cout << " failed" << endl;
                }
                else {
                    std::cout << " success" << endl;
                }
                std::vector<StationInfo> sites;
                for (const auto& item : root) {
                    StationInfo site;
                    site.stationID = item["StationID"].asInt();
                    site.name = item["name"].asString();
                    site.latitude = item["latitude"].asDouble();
                    site.longitude = item["longitude"].asDouble();
                    site.altitude = item["altitude"].asDouble();
                    site.TotalDiskSpace = item["TotalDiskSpace"].asInt();
                    site.FreeDiskSpace = item["FreeDiskSpace"].asInt();
                    site.isDeleted = item["isDeleted"].asBool();
                    site.gpsTime = item["GPSTime"].asString();
                    sites.push_back(site);
                }

                // 打印获取的站点信息
                for (const auto& site : sites) {
                    std::cout << "Name: " << site.name << std::endl;
                    std::cout << "StationID: " << site.stationID << std::endl;
                    std::cout << std::endl;
                }
            }
            else {
                std::cerr << "Failed to parse JSON response" << std::endl;
                std::cout << " :" << ":" << std::endl;
            }
        }catch (const Json::Exception& e) {
            // 捕获其他 Json::Exception 异常
            std::cout << "解析异常: " << e.what() << std::endl;
        }
    } else {
        std::cout << "HTTP request failed" << std::endl;
    }

    return 0;
}
