/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains class to interact with weather underground
 */

#ifndef SRC_LIB_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H_
#define SRC_LIB_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H_

#include <curl/curl.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <expected>
#include <list>
#include <map>
#include <regex>
#include <string>
#include <variant>
#include <vector>

using fmt::format;
using std::expected;
using std::find;
using std::get;
using std::map;
using std::regex;
using std::regex_match;
using std::string;
using std::unexpected;
using std::variant;
using std::vector;
using std::chrono::system_clock;

const string wu_url =
    "https://weatherstation.wunderground.com/weatherstation/"
    "updateweatherstation.php";
constexpr string url_separater = "&";

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
  /*
   * The order of the types in this variant must match the order of the values
   * in WuField_Type. The index() function of the variant returns which value
   * is currently being stored. So, 0 means string => WU_FIELD_TYPE_STRING.
   * It is important to keep these in sync. I hope to figure out a better way
   * to do that.
   */
  variant<float, string, system_clock::time_point>
      data;         // The original value passed
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

  static size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                              void* userp);

  expected<bool, int> setVarData(
      string field, variant<float, string, system_clock::time_point> value);

  expected<bool, int> setData(string field, string value);

  expected<bool, int> setData(string field, float value);

  expected<bool, int> setData(string field, system_clock value);

  void reset();

  string buildHttpRequest();

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

  map<string, WuFieldData> wu_data_;
  map<string, string> wu_text_data_ = {};
  map<string, float> wu_number_data_ = {};

  expected<bool, int> addData(
      string field, variant<float, string, system_clock::time_point> value);

  expected<WuFieldProperties, int> getFieldProperties(string field);
};

#endif  // SRC_LIB_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H
