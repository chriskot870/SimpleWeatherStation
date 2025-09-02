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

#include <expected>
#include <string>
#include <string_view>
#include <chrono>

#include "./weather_station.h"

constexpr std::string_view weather_station_config =
    "/usr/local/qw/etc/ws_config.json";
constexpr std::chrono::milliseconds ws_data_gathering_interval_default =
    std::chrono::milliseconds(10000);  // 10 seconds for default interval
constexpr std::chrono::milliseconds ws_data_gathering_interval_min =
    std::chrono::milliseconds(2500);  // 2.5 seconds is minimum gathering interval
constexpr std::chrono::milliseconds ws_data_gathering_interval_max =
    std::chrono::milliseconds(60000);  // 60 seconds is maximum gathering interval

class WeatherStationConfig {
 public:
  WeatherStationConfig();

  explicit WeatherStationConfig(const string& config_file);

  bool exists();

  std::expected<string, int> configVariablesFileName();

  void setConfigFile(const string& config_file);

  std::expected<bool, int> load();

  bool putRoot(const Json::Value& data);

  std::expected<string, int> softwareVersion();

  std::expected<string, int> model();

  std::expected<string, int> i2cBusName();

  std::expected<uint8_t, int> i2cDeviceAddress(string device);

  std::expected<std::chrono::milliseconds, int> getDataAcquisitionInterval();

  std::expected<string, int> getWuPwuName();

  std::expected<string, int> getWuPwuPassword();

  std::expected<std::chrono::milliseconds, int> getWuReportInterval();

  std::expected<bool, int> getWuReportEnabled();

  std::expected<string, int> getModbusSerialPort();

  std::expected<uint32_t, int> getEcowittLn90lpBaudRate();

  std::expected<uint8_t, int> getEcowittLn90lpAddress();


 private:
  string getLockFileName(string file);

  string config_file_;

  Json::Value config_json_;
  Json::Value variables_json_;
};

#endif  // SRC_INCLUDE_WEATHER_STATION_CONFIG_H_
