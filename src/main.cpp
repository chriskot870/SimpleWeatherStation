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
#include <systemd/sd-journal.h>

#include "include/lps22.h"
#include "include/sht4x.h"

#include "include/weather_underground.h"
#include "fmt/printf.h"
#include "fmt/chrono.h"

#include "fahrenheit.h"
#include "celsius.h"
#include "kelvin.h"
#include "inches_mercury.h"
#include "millibar.h"
#include "relative_humidity.h"

#include "dewpoint.h"

using std::cout;
using std::endl;
using std::string;
using fmt::format;
using qw_devices::I2cBus;
using qw_devices::I2cSht4x;
using qw_devices::kSht4xI2cPrimaryAddress;
using qw_devices::Lps22;
using qw_devices::kLps22hbI2cPrimaryAddress;
using qw_units::Fahrenheit;
using qw_units::Celsius;
using qw_units::Kelvin;
using qw_units::Millibar;
using qw_units::InchesMercury;
using std::get;
using std::holds_alternative;


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

  float valf, valc;
  //qw_units::Fahrenheit tmpf(45.3);
  Fahrenheit tempf(45);
  Celsius tempc(24);
  valf = tempf.value();

  tempf = tempc;

  valf = tempf.value();
  /*
   * Make sure the wu_id and wu_pwd are empty
   */
  wu_id.clear();
  wu_pwd.clear();

  while ((opt = getopt(argc, argv, "i:p:")) != -1) {
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
    sd_journal_print(LOG_ERR, "Initialization of lps22: %d\n", error);
    exit(1);
  }

  x_whoami = lps22.whoami();
  if (x_whoami.has_value() != true) {
    sd_journal_print(LOG_ERR, "Couldn't get Who am I value for lps22hb: %d\n", x_whoami.error());
    exit(1);
  }
  sd_journal_print(LOG_INFO, "LPS22HB who am I Value: 0x%x\n", x_whoami.value());

  /*
   * The sht4x device is connected to I2c bus 1 at the primary address
   */
  I2cSht4x sht4x(i2c_bus, kSht4xI2cPrimaryAddress);

  error = sht4x.softReset();
  if (error != 0) {
    sd_journal_print(LOG_ERR, "CHT4X reset failed/n");
    exit(1);
  }
  x_serial_number = sht4x.getSerialNumber();
  if (x_serial_number.has_value() == false) {
    sd_journal_print(LOG_ERR, "Getting SHT44 Serial Number failed: %d\n", x_serial_number.error());
    exit(1);
  }
  sd_journal_print(LOG_ERR, "SHT44 Serial Number: %d\n", x_serial_number.value());

  sd_journal_print(LOG_INFO, "Starting");

  //WeatherUnderground wu("KTXROANO168", "Password");

  WeatherUnderground wu("KTXROANO168", "HW0SG8q3");

  while (true) {
    /*
     * get the current time
     */
    auto now_time = std::chrono::system_clock::now();

    std::time_t now_t = std::chrono::system_clock::to_time_t(now_time);

    sd_journal_print(LOG_INFO, "%s",std::ctime(&now_t));

    /*
     * Gather up all the raw data
     */
    auto x_sht4x_temp =
        sht4x.getTemperatureMeasurement();

    auto x_sht4x_humidity = sht4x.getRelativeHumidityMeasurement();

    auto x_lps22_temp =
        lps22.getTemperatureMeasurement();

    auto x_lps22_pressure = lps22.getPressureMeasurement();

    /*
     * Put the raw data into the wu data
     */
    wu.setVarData("action", "updateraw");
    wu.setVarData("dateutc", "now");
    /*
     * Weather Underground wants fahrenheit
     */
    if (x_sht4x_temp.has_value()) {
      qw_units::Fahrenheit tempf = x_sht4x_temp.value().fahrenheitValue();
      wu.setVarData("tempf", tempf.value());
    }

    if (x_sht4x_humidity.has_value()) {
      qw_units::RelativeHumidity humidity = x_sht4x_humidity.value().relativeHumidityValue();
      wu.setVarData("humidity", humidity.value());
    }

    /*
     * If there are valid temperature and relative humidity then add a dewpoint
     */
    if (x_sht4x_temp.has_value() && x_sht4x_humidity.has_value()) {
      qw_units::Celsius tempc = x_sht4x_temp.value().celsiusValue();
      qw_units::RelativeHumidity humidity = x_sht4x_humidity.value().relativeHumidityValue();
      Celsius dewptc = qw_utilities::dewPoint(tempc, humidity);
      Fahrenheit dewptf = dewptc;
      wu.setVarData("dewptf", dewptf.value());
    }

    /*
     * Weather Underground wants inches mercury
     */
    if (x_lps22_pressure.has_value()) {
      qw_units::InchesMercury pressure = x_lps22_pressure.value().inchesMercuryValue();
      wu.setVarData("baromin", pressure.value());
    }

    /*
     * debug to check out the string
     */
    string http_request = wu.buildHttpRequest();
    sd_journal_print(LOG_INFO, "%s", http_request.c_str());
    
    auto errval = wu.sendData();
    if (errval.has_value() == false) {
      sd_journal_print(LOG_ERR, "COMM Error: %d", errval.error());
    }

    string response = wu.getHttpResponse();

    sd_journal_print(LOG_INFO, "%s", response.c_str());
    
    wu.reset();

    sleep(300);
  }
}
