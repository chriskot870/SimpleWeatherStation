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

#ifndef SRC_INCLUDE_WEATHER_UNDERGROUND_CONFIG_H_
#define SRC_INCLUDE_WEATHER_UNDERGROUND_CONFIG_H_

#include "weather_station.h"

constexpr int wu_default_report_interval = 300000;
constexpr int wu_report_interval_min = 2500;  // 2.5 seconds in milliseconds
constexpr int wu_report_interval_max =
    ((60 * 60) * 1000);  // 1 hour in milliseconds
/*
 * The ReportInterval is in milliseconds. 300000 = 5 minutes
 */
const std::string wu_default_config = R"({
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
  std::string getLockFileName(string file);

  std::string config_file_;
};

#endif  // SRC_INCLUDE_WEATHER_UNDERGROUND_CONFIG_H_