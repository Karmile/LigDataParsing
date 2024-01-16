#include "parse_json.h"
#include "iostream"

using namespace std;

int main() {
    // ����HTTPS�����ȡվ����Ϣ
    std::string str = (".\\config.yaml");
    Parser parser(str);

    vector<StationInfo> sites;
    vector<TriggerInfo> triggers;
    parser.parse_station_json(sites);
    parser.parse_trigger_json(triggers);

    return 0;
}