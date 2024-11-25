/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains class to send data to weather underground
 */
#include "include/weather_underground.h"

map<string, FieldType> wu_field_properties;

map<string, FieldType>  wu_fields = {
  {"ID", TEXT},
  {"PASSWORD", TEXT},
  {"dateutc", DATE},
  {"action", TEXT},
  {"winddir", NUMBER},
  {"windspeedmph", NUMBER},
  {"windgustmph", NUMBER},
  {"windgustdir", NUMBER},
  {"windspdmph_avg2m", NUMBER},
  {"winddir_avg2m", NUMBER},
  {"windgustmph_10m", NUMBER},
  {"windgustdir_10m", NUMBER},
  {"humidity", NUMBER},
  {"dewptf", NUMBER},
  {"tempf",  NUMBER},  // for extra outdoor temperatures use temp2f, temp3f etc...
  {"rainin", NUMBER},
  {"dailyrainin", NUMBER},
  {"baromin", NUMBER},
  {"weather", TEXT},
  {"clouds", TEXT},
  {"soiltempf", NUMBER},  // for extra soil temperature use soiltemp2f, soiltemp3f etc...
  {"soilmoisture", NUMBER},  // for extra sensors use soilmoisture2, soilmoisture3 etc...
  {"leafwetness", NUMBER},  // for extra sensors use leafwetness2, leafwetness3 etc...
  {"solarradiation", NUMBER},
  {"UV", NUMBER},
  {"visibility", NUMBER},
  {"indoortempf", NUMBER},
  {"indoorhumidity", NUMBER},
  /*
   * Pollution fields
   */
  {"AqNO", NUMBER},  // NO (nitric oxide) ppb
  {"AqNO2T", NUMBER},  // nitrogen dioxide, true measure ppb
  {"AqNO2", NUMBER},  // NO2 computed, NOx-NO ppb
  {"AqNO2Y", NUMBER},  // NO2 computed, NOy-NO ppb
  {"AqNOX", NUMBER},  // NOx (nitrogen oxides) - ppb
  {"AqNOY", NUMBER},  // NOy (total reactive nitrogen) - ppb
  {"AqNO3", NUMBER},  // NO3 ion (nitrate, not adjusted for ammonium ion) UG/M3
  {"AqSO4", NUMBER},  // SO4 ion (sulfate, not adjusted for ammonium ion) UG/M3
  {"AqSO2", NUMBER},  // (sulfur dioxide), conventional ppb
  {"AqSO2T", NUMBER},  // trace levels ppb
  {"AqCO", NUMBER},  // CO (carbon monoxide), conventional ppm
  {"AqCOT", NUMBER},  // CO trace levels ppb
  {"AqEC", NUMBER},  // EC (elemental carbon) – PM2.5 UG/M3
  {"AqOC", NUMBER},  // OC (organic carbon, not adjusted for oxygen and hydrogen) – PM2.5 UG/M3
  {"AqBC", NUMBER},  // BC (black carbon at 880 nm) UG/M3
  {"AqUV" , NUMBER}, // AETH  -UV-AETH (second channel of Aethalometer at 370 nm) UG/M3
  {"AqPM2.5", NUMBER},  // PM2.5 mass - UG/M3
  {"AqPM10", NUMBER},  // PM10 mass - PM10 mass
  {"AqOZONE", NUMBER}, // Ozone - ppb
  {"softwaretype", TEXT}  // [text] ie: WeatherLink, VWS, WeatherDisplay
};


WeatherUnderground::WeatherUnderground(string id, string password) : id_(id), password_(password) {}

size_t WeatherUnderground::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

expected<bool, int> WeatherUnderground::sendData() {
  
  /*
   * We need to check that ID and PASSWORD are NOT in the map
   */
  if (wu_text_data_.contains("ID") || wu_text_data_.contains("PASSWORD")) {
    return unexpected(EINVAL);
  }

  /*
   * We have to make sure that these values exist in the map
   */
  if ((wu_text_data_.contains("action") == false) || (wu_text_data_["action"] != "updateraw") || (wu_text_data_.contains("dateutc")== false)) {
    return unexpected(EINVAL);
  }

  http_get_request_ = buildHttpRequest();

  CURL *curl = curl_easy_init();
  CURLcode res;
  /*
   * Set the URL options
   */
  curl_easy_setopt(curl, CURLOPT_URL, http_get_request_.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WeatherUnderground::WriteCallback);
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

expected<bool, int> WeatherUnderground::setData(string field, string value) {
  int n;
  /*
   * Check if the field is on the wu fields list
   */
  if (wu_fields.contains(field) == false) {
    return unexpected(EINVAL);
  }

    /*
     * Make sure it is a TEXT field
     */
    if (wu_fields[field] != TEXT) {
      /*
       * The dateutc has type DATE but can be "now"
       */
      if (field != "dateutc" || value != "now") {
        unexpected(EINVAL);
      }
    }

    wu_text_data_[field] = value;

  return true;
}

expected<bool, int> WeatherUnderground::setData(string field, float value) {
  int n;
    /*
     * Check if the field is on the wu fields list
     */
    if (wu_fields.contains(field) == false) {
      return unexpected(EINVAL);
    }  

    /*
     * Make sure it is a NUMBER field
     */
    
    if (wu_fields[field] != NUMBER) {
      return unexpected(EINVAL);
    }
    
    wu_number_data_[field] = value;

  return true;
}

expected<bool, int> WeatherUnderground::setData(string field, system_clock value) {

        /*
     * Check if the field is on the wu fields list
     */
    if (wu_fields.contains(field) == false) {
      return unexpected(EINVAL);
    }  

    /*
     * Make sure it is a TEXT field
     */
    /*
    if (wu_fields[field] != DATE) {
       return unexpected(EINVAL);
    }
     */

    /*
     * This is a date and we will store it as a TEXT
     */ 
    /*
    auto const display_time = system_clock::to_time_t(value);

    wu_text_data_[field] = value;
     */

  return true;
}

void WeatherUnderground::clearData() {

    wu_number_data_.clear();
    wu_text_data_.clear();
    clearHttpResponse();
    clearHttpRequest();

    return;
}

string WeatherUnderground::buildHttpRequest() {
  string url_get_string;
  string number_value;
  char *escaped_value;

  /*
   * Escape get parameters
   */
  CURL *curl = curl_easy_init();
  /*
   * First we need to build the string with the values
   * We start with the url and ID and password. Then loop through the fields
   */
  url_get_string += url_separater + "ID=";
  url_get_string.append(curl_easy_escape(curl,id_.c_str(), id_.length()));
  url_get_string += url_separater + "PASSWORD=";
  url_get_string.append(curl_easy_escape(curl, password_.c_str(), password_.length()));

  /*
   * Now walk through the possible fields and see if the values have been set
   */

  for (auto [key, value] : wu_fields) {
    switch (value) {
        case TEXT :
        case DATE :
            if (wu_text_data_.contains(key)) {
                url_get_string += url_separater + key + "=";
                escaped_value = curl_easy_escape(curl, wu_text_data_[key].c_str(), wu_text_data_[key].length());
                url_get_string.append(curl_easy_escape(curl, escaped_value, strlen(escaped_value)));
            }
        break;
        case NUMBER :
            if (wu_number_data_.contains(key)) {
                url_get_string += url_separater + key + "=";
                number_value = fmt::format("{:.2f}", wu_number_data_[key]);
                escaped_value = curl_easy_escape(curl, number_value.c_str(), number_value.length());
                url_get_string.append(escaped_value, strlen(escaped_value));
            }
            break;
    }
  }

  /*
   * Put the URL pieces together
   */
  string url_http_string = wu_url + "?";
  url_http_string.append(url_get_string);

  curl_easy_cleanup(curl);

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
