/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains data for the weather station
 */

#ifndef SRC_INCLUDE_WEATHER_STATION_CONFIG_H_
#define SRC_INCLUDE_WEATHER_STATION_CONFIG_H_

using std::string;

const string weather_station_config = "/usr/local/qw/etc/ws_config.json";
const string ws_lock_directory = "/run/lock/";
const string ws_lock_file_suffix = ".lock";

constexpr int ws_report_interval_min = 2500;  // 2.5 seconds in milliseconds
constexpr int ws_report_interval_max =
    ((60 * 60) * 1000);  // 1 hour in milliseconds
/*
 * The ReportInterval is in milliseconds. 300000 = 5 miuntes
 */
const string ws_default_config = R"({
    "WeatherUnderground": {
        "pwu_name": "KTXROANO168",
        "pwu_password": "HW0SG8q3"
    },
   "ReportInterval": 300000
})";

class WeatherStationConfig {
 public:
  WeatherStationConfig();

  WeatherStationConfig(const string& config_file);

  bool exists();

  bool initialize();

  void setConfigFile(string config_file);

  bool getRoot(Json::Value& ws_access);

  bool putRoot(Json::Value data);

 private:
  string getLockFileName(string file);

  string config_file_;
};

#endif  // SRC_INCLUDE_WEATHER_STATION_CONFIG_H_