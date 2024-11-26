/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * The main program to show the data.
 * Right now it's pretty simple
 */
#include <unistd.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "include/lps22.h"
#include "include/sht4x.h"
#include "relative_humidity_datum.h"
#include "pressure_datum.h"
#include "temperature_datum.h"
#include "dew_point.h"
#include "include/weather_underground.h"

using std::cout;
using std::endl;
using std::string;

int main(int argc, char** argv) {
  string temperature;
  string humidity;
  float ctemp, pressure, hum, ftemp, sht44temp, lps22temp;
  string time_string;
  std::expected<uint8_t, int> x_whoami;
  std::expected<uint32_t, int> x_serial_number;
  int error;
  int opt;
  string wu_id;
  string wu_pwd;

  /*
   * Make sure the wu_id and wu_pwd are empty
   */
  wu_id.clear();
  wu_pwd.clear();

  while ((opt = getopt(argc, argv, "1:p:")) != -1) {
    switch (opt) {
      case 'p' :
        wu_pwd.append(optarg);
        break;
      case 'i':
        wu_id.append(optarg);
        break;
    }
  }

  I2cBus i2c_bus = I2cBus(string("/dev/i2c-1"));

  Lps22 lps22(i2c_bus, kLps22hbI2cPrimaryAddress);

  error = lps22.init();
  if (error != 0) {
    printf("Initialization of lps22: %d\n", error);
    exit(1);
  }

  x_whoami = lps22.whoami();
  if (x_whoami.has_value() != true) {
    printf("Couldn't get Who am I value for lps22hb: %d\n", x_whoami.error());
    exit(1);
  }
  printf("LPS22HB who am I Value: 0x%x\n", x_whoami.value());

  /*
   * The sht4x device is connected to I2c bus 1 at the primary address
   */
  I2cSht4x sht4x(i2c_bus, kSht4xI2cPrimaryAddress);

  error = sht4x.softReset();
  if (error != 0) {
    printf("CHT4X reset failed/n");
    exit(1);
  }
  x_serial_number = sht4x.getSerialNumber();
  if (x_serial_number.has_value() == false) {
    printf("Getting SHT44 Serial Number failed: %d\n", x_serial_number.error());
    exit(1);
  }
  printf("SHT44 Serial Number: %d\n", x_serial_number.value());

  cout << "Starting" << endl;

  //WeatherUnderground wu("KTXROANO168", "Password");

  WeatherUnderground wu(wu_id, wu_pwd);

  while (true) {
    /*
     * get the current time
     */
    auto now_time = std::chrono::system_clock::now();

    /*
     * Gather up all the raw data
     */
    auto x_sht4x_temp =
        sht4x.getTemperatureMeasurement(TEMPERATURE_UNIT_FAHRENHEIT);

    auto x_sht4x_humidity = sht4x.getRelativeHumidityMeasurement();

    auto x_lps22_temp =
        lps22.getTemperatureMeasurement(TEMPERATURE_UNIT_FAHRENHEIT);

    auto x_lps22_pressure = lps22.getPressureMeasurement(PRESSURE_UNIT_INCHES_MERCURY);

    /*
     * Put the raw data into the wu data
     */
    wu.setData("action", "updateraw");
    wu.setData("dateutc", "now");
    if (x_sht4x_temp.has_value()) {
      wu.setData("tempf", x_sht4x_temp.value().getDatum().getValue());
    }

    if (x_sht4x_humidity.has_value()) {
      wu.setData("humidity", x_sht4x_humidity.value().getDatum().getValue());
    }

    /*
     * The dew point depends on temperature and humidity so make sure they exist
     */
    if (x_sht4x_temp.has_value() && x_sht4x_humidity.has_value()) {
      auto dew_point = getDewPointMeasurement(x_sht4x_temp.value(), x_sht4x_humidity.value());
      if (dew_point.has_value()) {
        wu.setData("dewpoint", dew_point.value().getDatum().getValue());
      }
    }

    if (x_lps22_pressure.has_value()) {
      wu.setData("baromin", x_lps22_pressure.value().getDatum().getValue());
    }

    /*
     * debug to check out the string
     */
    string http_request = wu.buildHttpRequest();
    printf("Sending HTTP Request: %s\n", http_request.c_str());

    auto errval = wu.sendData();
    if (errval.has_value() == false) {
      printf("COMM Error: %d", errval.error());
    }

    string response = wu.getHttpResponse();

    printf("Response: %s", response.c_str());

    wu.reset();

    sleep(300);
  }
}
