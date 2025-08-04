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
 * This contains class to send data to weather underground
 */
#include "include/weather_underground.h"

#include <expected>

#include <algorithm>
#include <map>
#include <string>

#include "qw/logger/include/logger.h"

using std::find;
using std::regex_match;
using std::string;
using qw::logging::logger;

/*
 * Any filed that matches the pattern of the key has the associated properties
 */
const map<string, WuFieldProperties> wu_field_regex_list = {
    {"^ID$", WuFieldProperties(WU_FIELD_TYPE_STRING, "{}")},
    {"^PASSWORD$", WuFieldProperties(WU_FIELD_TYPE_STRING, "{}")},
    {"^dateutc$", WuFieldProperties(WU_FIELD_TYPE_SYSTEM_CLOCK_TIME_POINT,
                                    "{:%Y-%m-%d %H:%M:%S}")},
    {"^action$", WuFieldProperties(WU_FIELD_TYPE_STRING, "{}")},
    {"^baromin$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{0:.2f}")},
    {"^humidity$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{0:.2f}")},
    {"^temp[2-9]?f$|^temp[1-9][0-9]f$",
     WuFieldProperties(WU_FIELD_TYPE_NUMBER,
                       "{0:.2f}")},  // This supports tempf, temp2-99f
    {"^dewptf$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{0:.2f}")},
    {"^windspeedmph$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{0:.1f}")},
    {"^windgustmph$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{0:.1f}")},
    {"^windspdmph_avg2m$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{0:.1f}")},
    {"^windgustmph_10m$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{0:.1f}")}};

const map<string, string> wu_format_regex_list = {
    {"^ID$", "{}"},
    {"^PASSWORD$", "{}"},
    {"^dateutc$", "{:%Y-%m-%d %H:%M:%S}"},
    {"^action$", "{}"},
    {"^baromin$", "{0:.2f}"},
    {"^humidity$", "{0:.2f}"},
    {"^temp[2-9]?f$|^temp[1-9][0-9]f$",
     "{0:.2f}"},  // This supports tempf, temp2-99f
    {"^dewptf$", "{0:.2f}"},
    {"^windspeedmph$", "{0:.1f}"},
    {"^windgustmph$", "{0:.1f}"},
    {"^windspdmph_avg2m$", "{0:.1f}"},
    {"^windgustmph_10m$", "{0:.1f}"}};

const vector<string_view> wu_fields = {
    "ID", "PASSWORD", "dateutc", "action", "winddir", "windspeedmph",
    "windgustmph", "windgustdir", "windspdmph_avg2m", "winddir_avg2m",
    "windgustmph_10m", "windgustdir_10m", "humidity", "dewptf",
    "tempf",  // for extra outdoor temperatures use temp2f, temp3f etc...
    "rainin", "dailyrainin", "baromin", "weather", "clouds",
    "soiltempf",  // for extra soil temperature use soiltemp2f, soiltemp3f etc...
    "soilmoisture",  // for extra sensors use soilmoisture2, soilmoisture3 etc...
    "leafwetness",   // for extra sensors use leafwetness2, leafwetness3 etc...
    "solarradiation", "UV", "visibility", "indoortempf", "indoorhumidity",
    /*
     * Pollution fields
     */
    "AqNO",    // NO (nitric oxide) ppb
    "AqNO2T",  // nitrogen dioxide, true measure ppb
    "AqNO2",   // NO2 computed, NOx-NO ppb
    "AqNO2Y",  // NO2 computed, NOy-NO ppb
    "AqNOX",   // NOx (nitrogen oxides) - ppb
    "AqNOY",   // NOy (total reactive nitrogen) - ppb
    "AqNO3",   // NO3 ion (nitrate, not adjusted for ammonium ion) UG/M3
    "AqSO4",   // SO4 ion (sulfate, not adjusted for ammonium ion) UG/M3
    "AqSO2",   // (sulfur dioxide), conventional ppb
    "AqSO2T",  // trace levels ppb
    "AqCO",    // CO (carbon monoxide), conventional ppm
    "AqCOT",   // CO trace levels ppb
    "AqEC",    // EC (elemental carbon) – PM2.5 UG/M3
    "AqOC",  // OC (organic carbon, not adjusted for oxygen and hydrogen) – PM2.5 UG/M3
    "AqBC",  // BC (black carbon at 880 nm) UG/M3
    "AqUV",  // AETH  -UV-AETH (second channel of Aethalometer at 370 nm) UG/M3
    "AqPM2.5",      // PM2.5 mass - UG/M3
    "AqPM10",       // PM10 mass - PM10 mass
    "AqOZONE",      // Ozone - ppb
    "softwaretype"  // [text] ie: WeatherLink, VWS, WeatherDisplay
};

WeatherUnderground::WeatherUnderground(string id, string password)
    : id_(id), password_(password) {}

size_t WeatherUnderground::writeCallback(void* contents, size_t size,
                                         size_t nmemb, void* userp) {
  (reinterpret_cast<string*>(userp))
      ->append(reinterpret_cast<char*>(contents), size * nmemb);
  return size * nmemb;
}

expected<bool, int> WeatherUnderground::sendData() {
  /*
   * We need to check that ID and PASSWORD are NOT in the map
   */
  if (wu_data_.contains("ID") || wu_data_.contains("PASSWORD")) {
    return unexpected(EINVAL);
  }

  /*
   * We have to make sure that these values exist in the map
   */
  if ((wu_data_.contains("action") == false) ||
      (wu_data_["action"].data != "updateraw") ||
      (wu_data_.contains("dateutc") == false)) {
    return unexpected(EINVAL);
  }

  expected<string, int> http_get_request_ = buildHttpRequest();
  if (http_get_request_.has_value() != true) {
    logger.log(LOG_INFO, "Couldn't build HTTP request");
    return unexpected(http_get_request_.error());
  }

  /*
   * Now use curl to send the HTTP GET request
   */
  CURL* curl = curl_easy_init();
  CURLcode res;

  /*
   * Set the URL options
   */
  curl_easy_setopt(curl, CURLOPT_URL, http_get_request_.value().c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                   WeatherUnderground::writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_);

  /*
   * Send the GET request
   */
  res = curl_easy_perform(curl);

  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    /*
     * May want to return different values for different return errors
     */
    return unexpected(ECOMM);
  }

  return true;
}

expected<bool, int> WeatherUnderground::setVarData(string_view field,
                                                   string_view value) {

  /*
   * We only get ID and PASSWORD from id_ and password_ when class is created.
   */
  if ((field == "ID") || field == "PASSWORD") {
    return unexpected(EINVAL);
  }

  /*
   * Check if the field is on the wu_fields list
   */
  auto it = find(wu_fields.begin(), wu_fields.end(), field);
  if (it == wu_fields.end()) {
    return unexpected(EINVAL);
  }

  /*
   * It is my understanding the action can only be updateraw
   */
  if (field == "action") {
    if (value != "updateraw") {
      return unexpected(EINVAL);
    }
  }

  /*
   * dateutc we only support now for now
   */
  if (field == "dateutc") {
    if (value != "now") {
      return unexpected(EINVAL);
    }
  }

  /*
   * It's OK to add the field and it's value
   */
  expected<void, int> add = addData(field, value);
  if (add.has_value() == false) {
    logger.log(LOG_INFO, format("Couldn't add any data for field {}", field));
    return unexpected(add.error());
  }

  return true;
}

expected<void, int> WeatherUnderground::addData(string_view field, string_view value) {
  /*
   * Create the field's data and set the value to what was passed in
   */
  WuFieldData field_data;
  field_data.data = value;

  /*
   * Now escape the string and store it in the url_data field
   */
  CURL* curl = curl_easy_init();
  if (nullptr == curl) {
    logger.log(LOG_INFO, "Unable to create curl ptr");
    return unexpected(ENOMEM);
  }

  field_data.url_data =
      string(curl_easy_escape(curl, value.data(), value.length()));
  curl_easy_cleanup(curl);

  /*
   * Now store the field data into the wu_data_ array
   */
  wu_data_[field.data()] = field_data;

  // Could use return std::expected<voide, int>(std::in_place);
  // The {} is more concise.
  return {};
}

expected<string, int> WeatherUnderground::getFieldFormat(string_view field) {

  for (auto [rgx, field_format] : wu_format_regex_list) {
    if (regex_match(field.data(), regex(rgx)) == true) {
      return field_format;
    }
  }

  return unexpected(EINVAL);
}

void WeatherUnderground::reset() {
  wu_data_.clear();
  clearHttpResponse();
  clearHttpRequest();

  return;
}

expected<string, int> WeatherUnderground::buildHttpRequest() {
  string url_get_string;

  /*
   * Add the ID and PASSWORD
   */
  expected<void, int> add = addData("ID", id_);
  if (add.has_value() != true) {
    logger.log(LOG_INFO, "Couldn't add ID field");
    return unexpected(add.error());
  }
  add = addData("PASSWORD", password_);
  if (add.has_value() != true) {
    logger.log(LOG_INFO, "Couldn't add PASSWORD field");
    return unexpected(add.error());
  }

  /*
   * Now walk through the url data map and create the url escaped get string.
   */
  for (auto [field, value] : wu_data_) {
    url_get_string += format("&{}={}", field, value.url_data);
  }

  /*
   * Put the URL pieces together
   */
  string url_http_string(wu_url);
  url_http_string += "?" + url_get_string;

  return url_http_string;
}

string WeatherUnderground::getHttpRequest() {
  return http_get_request_;
}

void WeatherUnderground::clearHttpRequest() {
  http_get_request_.clear();
  return;
}

string WeatherUnderground::getHttpResponse() {
  return *response_;
}

void WeatherUnderground::clearHttpResponse() {
  response_->clear();

  return;
}
