#ifndef SRC_INCLUDE_WEATHERUNDERGROUND_CONFIG_H_
#define SRC_INCLUDE_WEATHERUNDERGROUND_CONFIG_H_

using std::string;

constexpr int wu_default_report_interval = 300000;
constexpr int wu_report_interval_min = 2500;  // 2.5 seconds in milliseconds
constexpr int wu_report_interval_max =
    ((60 * 60) * 1000);  // 1 hour in milliseconds
/*
 * The ReportInterval is in milliseconds. 300000 = 5 miuntes
 */
const string wu_default_config = R"({
    "WeatherUnderground": {
        "pwu_name": "KTXROANO168",
        "pwu_password": "HW0SG8q3"
    },
   "report_interval": 300000
})";

class WeatherUndergroundConfig {
 public:

  WeatherUndergroundConfig(const string& config_file);

  bool exists();

  bool initialize();

  void setConfigFile(string config_file);

  bool getRoot(Json::Value& ws_access);

  bool putRoot(Json::Value data);

 private:
  string getLockFileName(string file);

  string config_file_;
};

#endif  // SRC_INCLUDE_WEATHERUNDERGROUND_CONFIG_H_