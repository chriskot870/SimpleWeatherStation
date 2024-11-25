/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains class to interact with weather underground
 */

#ifndef SRC_LIB_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H_
#define SRC_LIB_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H_

#include <string>
#include <algorithm>
#include <fmt/format.h>
#include <expected>
#include <chrono>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <cstring>

using std::string;
using std::expected;
using std::unexpected;
using std::vector;
using std::map;
using std::chrono::system_clock;
using std::find;
//using fmt::v9::format;

const string wu_url = "https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php";
constexpr string url_separater = "&";

/*
 * These are the fields that weather underground recognizes
 * See https://support.weather.com/s/article/PWS-Upload-Protocol?language=en_US
 */

enum FieldType {
    NUMBER,
    TEXT,
    DATE
};

struct WuField {
    string name;
    FieldType type;
};

//map<string, FieldType> wu_fields;

class WeatherUnderground {
 public:

  WeatherUnderground(string id, string password);

  static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

  expected<bool, int> setData(string field, string value);

  expected<bool, int> setData(string field, float value);

  expected<bool, int> setData(string field, system_clock value);

  void clearData();

  string buildHttpRequest();

  expected<bool, int> sendData();

  string getHttpResponse();

  void clearHttpResponse();

  string getHttpRequest();

  void clearHttpRequest();

 private:

  string id_;
  string password_;
  string* response_  = new string();
  string http_get_request_;

  map<string, string> wu_text_data_ = {};
  map<string, float> wu_number_data_ = {};

};

#endif // SRC_LIB_WEATHER_UNDERGROUND_INCLUDE_WEATHER_UNDERGROUND_H
