/*
 * Copyright 2024,2025, Chris Kottaridis. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * NTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This contains data for the weather station
 */

#ifndef SRC_INCLUDE_WEATHER_STATION_CONFIG_H_
#define SRC_INCLUDE_WEATHER_STATION_CONFIG_H_

#include <string>
#include <string_view>

#include "./weather_station.h"

constexpr std::string_view weather_station_config = "/usr/local/qw/etc/ws_config.json";

class WeatherStationConfig {
 public:
  WeatherStationConfig();

  explicit WeatherStationConfig(const string& config_file);

  bool exists();

  void setConfigFile(const string& config_file);

  bool getRoot(Json::Value& ws_access);

  bool putRoot(Json::Value data);

 private:
  string getLockFileName(string file);

  string config_file_;
};

#endif  // SRC_INCLUDE_WEATHER_STATION_CONFIG_H_