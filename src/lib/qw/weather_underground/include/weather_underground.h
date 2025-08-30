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
 * This contains class to interact with weather underground
 */

#ifndef SRC_LIB_QW_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H_
#define SRC_LIB_QW_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H_

#include <cstring>

#include <algorithm>
#include <array>
#include <chrono>
#include <expected>  // cpplint thinks this is a c system header // NOLINT
#include <list>
#include <map>
#include <regex>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "curl/curl.h"
#include "fmt/chrono.h"
#include "fmt/format.h"

#include "qw/units/direction/include/degrees.h"
#include "qw/units/direction/include/direction.h"
#include "qw/units/humidity/include/relative_humidity.h"
#include "qw/units/include/unit_measurement.h"
#include "qw/units/light/include/light.h"
#include "qw/units/light/include/lux.h"
#include "qw/units/pressure/include/inches_mercury.h"
#include "qw/units/pressure/include/pressure.h"
#include "qw/units/speed/include/miles_per_hour.h"
#include "qw/units/speed/include/speed.h"
#include "qw/units/temperature/include/fahrenheit.h"
#include "qw/units/temperature/include/temperature.h"
#include "qw/units/uvi/include/uvi.h"

using fmt::format;
using qw::units::Degrees;
using qw::units::Direction;
using qw::units::Fahrenheit;
using qw::units::InchesMercury;
using qw::units::Light;
using qw::units::Lux;
using qw::units::MeasurementHistory;
using qw::units::MilesPerHour;
using qw::units::Pressure;
using qw::units::RelativeHumidity;
using qw::units::Speed;
using qw::units::Temperature;
using qw::units::Uvi;
using std::expected;
using std::find;
using std::get;
using std::map;
using std::regex;
using std::regex_match;
using std::string;
using std::string_view;
using std::unexpected;
using std::variant;
using std::vector;
using std::chrono::system_clock;

using qw::units::UnitMeasurement;

constexpr string_view wu_url =
    "https://weatherstation.wunderground.com/weatherstation/"
    "updateweatherstation.php";
constexpr string_view url_separater = "&";

/*
 * These are the fields that weather underground recognizes
 * See https://support.weather.com/s/article/PWS-Upload-Protocol?language=en_US
 */
enum FieldType {  // When we switch to WuFieldType below this will not be needed
  NUMBER,
  TEXT,
  DATE
};

enum WuFieldType {
  WU_FIELD_TYPE_NUMBER,
  WU_FIELD_TYPE_STRING,
  WU_FIELD_TYPE_SYSTEM_CLOCK_TIME_POINT,
  WU_FIELD_TYPE_INVALID  // This should always be last and indicates an invalid type.
                         // Be sure to add any new values above this
                         // It is used in setVarData to indicate that an regex
                         // was not matched
};

class WuFieldProperties {
 public:
  WuFieldType type_;
  string default_format_;

  /*
   * Define the different Consructors
   */
  WuFieldProperties() {}
  WuFieldProperties(WuFieldType type, string default_format)
      : type_(type), default_format_(default_format) {}
};

struct WuFieldData {
  string data;      // The original value passed
  string url_data;  // The URL escaped string
};

/*
 * A field that matches the regex expression has the indicated WuFieldType
 */
class WuRegexTypeAssignment {
 public:
  string expression;
  WuFieldType type;
};

class WeatherUnderground {
 public:
  WeatherUnderground(string id, string password);

  static size_t writeCallback(void* contents, size_t size, size_t nmemb,
                              void* userp);

  expected<string, int> getFieldFormat(string_view field);

  expected<bool, int> setVarData(string_view field, string_view value);

  void addTemperatureMeasurements(vector<UnitMeasurement<Temperature>> m_temps);

  void addRelativeHumidityMeasurement(UnitMeasurement<RelativeHumidity> m_rh);

  void addPressureMeasurement(UnitMeasurement<Pressure> m_pressure);

  void addWindSpeedMeasurement(MeasurementHistory<Speed>& m_wind_speed_history);

  void addWindDirectionMeasurement(MeasurementHistory<Direction>& m_wind_direction_history);

  void addUviMeasurement(UnitMeasurement<Uvi> m_uvi);

  void addLightMeasurement(UnitMeasurement<Light> m_light);

  void reset();

  expected<string, int> buildHttpRequest();

  expected<bool, int> sendData();

  string getHttpResponse();

  void clearHttpResponse();

  string getHttpRequest();

  void clearHttpRequest();

 private:
  string id_;
  string password_;
  string* response_ = new string();
  string http_get_request_;

  expected<void, int> addData(string_view field, string_view value);

  map<string, WuFieldData> wu_data_;
};

#endif  // SRC_LIB_QW_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H_
