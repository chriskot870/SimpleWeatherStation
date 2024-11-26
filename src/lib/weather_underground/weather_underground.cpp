/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains class to send data to weather underground
 */
#include "include/weather_underground.h"

/*
 * Any filed that matches the pattern of the key has the associated properties
 */
const map<string, WuFieldProperties> wu_field_regex_list = {
  {"^ID$", WuFieldProperties(WU_FIELD_TYPE_STRING, "{}")},
  {"^PASSWORD$", WuFieldProperties(WU_FIELD_TYPE_STRING, "{}")},
  {"^dateutc$", WuFieldProperties(WU_FIELD_TYPE_SYSTEM_CLOCK_TIME_POINT, "{:%Y-%m-%d %H:%M:%S}")},
  {"^action$", WuFieldProperties(WU_FIELD_TYPE_STRING, "{}")},
  {"^baromin$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{}")},
  {"^humidity$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{}")},
  {"^temp[2-9]?f$|^temp[1-9][0-9]f$", WuFieldProperties(WU_FIELD_TYPE_NUMBER, "{}")}  // This supports tempf, temp2-99f
};

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

  /*
   * Now use curl to send the HTTP GET request
   */
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

expected<bool, int> WeatherUnderground::setVarData(string field, variant<float, string, system_clock::time_point> value) {

  /*
   * Sanity check by walking through the regex list to make sure it is a valid field
   */
  expected<WuFieldProperties, int> field_properties = getFieldProperties(field);
  if (field_properties.has_value() == false) {
    return unexpected(field_properties.error());
  }

  /*
   * We only get ID and PASSWORD from id_ and password_ when class is created.
   */
  if ((field == "ID") || field == "PASSWORD") {
    return unexpected(EINVAL);
  }
  /*
   * It is my understanding the action can only be updateraw
   */
  if (field == "action") {
    if ((value.index() != WU_FIELD_TYPE_STRING) || (get<string>(value) != "updateraw")) {
      return unexpected(EINVAL);
    }
  }

  /*
   * dateutc can either be system_clock::time_point or "now"
   * So it's type check needs to be done separately
   */
  if (field == "dateutc") {
    if ((value.index() != WU_FIELD_TYPE_SYSTEM_CLOCK_TIME_POINT) || (value.index() != WU_FIELD_TYPE_STRING)) {
      return unexpected(EINVAL);
    }
    if ((value.index() == WU_FIELD_TYPE_STRING) && (get<string>(value) != "now")) {
      return unexpected(EINVAL);
    }
  } else {
    /*
     * Make sure all the other fields are passing the correct type
     */
    if (field_properties.value().type_ != value.index()) {
      return unexpected(EINVAL);
    }
  }

  /*
   * It's OK to add the field and it's value
   */
  addData(field, value);

  return true;
}

  expected<bool, int> WeatherUnderground::addData(string field, variant<float, string, system_clock::time_point> value) {
  /*
   * This is a private routine. The public routine setData does all the sanity checking
   * And then calls this routine to actually add the field data to the map.
   * We assume that field and value a valid when we get here.
   * This allows us to add ID and PASSWORD when we send the data.
   */
  
  
  /*
   * Create the field's data and set the value to what was passed in
   */
  WuFieldData field_data;
  field_data.data = value;
  
  /*
   * Get the field's properties in order to get the default format
   */
  expected<WuFieldProperties, int> field_properties = getFieldProperties(field);
  if (field_properties.has_value() == false) {
    return unexpected(field_properties.error());
  }
 
  /*
   * Get the data string to be converted to escaped URL
   * Define data_string before switch so it won't go out of scope.
   * You need to use fmt::runtime() to use a variable for the
   * format string.
   */
  string data_string;
  switch (value.index()) {
    case WU_FIELD_TYPE_NUMBER :
      data_string = format(fmt::runtime(field_properties.value().default_format_), get<float>(value));
      break;
    case WU_FIELD_TYPE_STRING :
      data_string = format(fmt::runtime(field_properties.value().default_format_), get<string>(value));
      break;
    case WU_FIELD_TYPE_SYSTEM_CLOCK_TIME_POINT :
      data_string = format(fmt::runtime(field_properties.value().default_format_), get<system_clock::time_point>(value));
      break;
  }

  /*
   * Now escape the string and store it in the url_data field
   */
  CURL *curl = curl_easy_init();
  field_data.url_data = string(curl_easy_escape(curl, data_string.c_str(), data_string.length()));
  curl_easy_cleanup(curl);

  /*
   * Now store the field data into the wu_data_ array
   */
  wu_data_[field] = field_data;

  return true;
}

expected<WuFieldProperties, int> WeatherUnderground::getFieldProperties(string field) {

  for (auto [rgx, properties] : wu_field_regex_list) {
    if (std::regex_match(field, regex(rgx)) == true) {
      return properties;
      break;
    }
  }

  return unexpected(EINVAL);
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

void WeatherUnderground::reset() {

    wu_data_.clear();
    wu_number_data_.clear();
    wu_text_data_.clear();
    clearHttpResponse();
    clearHttpRequest();

    return;
}

string WeatherUnderground::buildHttpRequest() {
  string url_get_string;

  /*
   * Add the ID and PASSWORD
   */
  addData("ID", id_);
  addData("PASSWORD", password_);

  /*
   * Now walk through the url data map and create the urk escaped get string.
   */
  for (auto [field, value] : wu_data_) {
    url_get_string += format("&{}={}", field, value.url_data);
  }

  /*
   * Put the URL pieces together
   */
  string url_http_string = wu_url + "?" + url_get_string;

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
