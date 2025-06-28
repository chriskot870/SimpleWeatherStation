/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains data for the weather station
 */

#ifndef SRC_INCLUDE_WEATHER_STATION_CONFIG_H_
#define SRC_INCLUDE_WEATHER_STATION_CONFIG_H_

#include "weather_station.h"

using std::string;

const string weather_station_config = "/usr/local/qw/etc/ws_config.json";

class WeatherStationConfig {
 public:
  WeatherStationConfig();

  WeatherStationConfig(const string& config_file);

  bool exists();

  void setConfigFile(string config_file);

  bool getRoot(Json::Value& ws_access);

  bool putRoot(Json::Value data);

 private:
  string getLockFileName(string file);

  string config_file_;
};

#endif  // SRC_INCLUDE_WEATHER_STATION_CONFIG_H_