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
 * The main program to show the data.
 * Right now it's pretty simple
 */

#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

#include "fmt/chrono.h"
#include "fmt/format.h"

#include "include/weather_station.h"
#include "include/weather_station_config.h"
#include "include/weather_underground.h"
#include "include/weather_underground_config.h"
#include "qw/devices/i2c/include/ads1015.h"
#include "qw/devices/i2c/include/lps22.h"
#include "qw/devices/i2c/include/sht4x.h"
#include "qw/devices/include/anomometer_adafruit.h"
#include "qw/locking/include/locking_file.h"
#include "qw/logger/include/logger.h"
#include "qw/systemd/include/sd_service_unit.h"
#include "qw/systemd/include/sd_unit.h"
#include "qw/systemd/include/systemd.h"
#include "qw/units/humidity/include/relative_humidity.h"
#include "qw/units/pressure/include/inches_mercury.h"
#include "qw/units/speed/include/miles_per_hour.h"
#include "qw/units/temperature/include/fahrenheit.h"
#include "qw/weather/include/dewpoint.h"
#include "qw/weather/include/windspeed_history.h"

using fmt::format;
using qw::devices::ADS1015_MUX_AIN0_GND;
using qw::devices::Ads1015Config;
using qw::devices::AnomometerAdafruit;
using qw::devices::I2cAds1015;
using qw::devices::I2cBus;
using qw::devices::I2cSht4x;
using qw::devices::kAds1015I2cPrimaryAddress;
using qw::devices::kLps22hbI2cPrimaryAddress;
using qw::devices::kSht4xI2cPrimaryAddress;
using qw::devices::Lps22;
using qw::logging::Logger;
using qw::logging::logger;
using qw::logging::LOGGER_DEBUG;
using qw::logging::LOGGER_ERR;
using qw::logging::LOGGER_INFO;
using qw::logging::LOGGER_MODE_FILE;
using qw::logging::LOGGER_MODE_JOURNAL;
using qw::logging::LOGGER_MODE_NOLOGGING;
using qw::systemd::SdBusError;
using qw::systemd::SdServiceUnit;
using qw::systemd::SdUnit;
using qw::systemd::systemd_destination;
using qw::systemd::systemd_quietwind_service_path;
using qw::systemd::systemd_service_interface;
using qw::systemd::systemd_unit_interface;
using qw::units::Celsius;
using qw::units::Fahrenheit;
using qw::units::InchesMercury;
using qw::units::Kelvin;
using qw::units::KilometersPerHour;
using qw::units::MilesPerHour;
using qw::units::Millibar;
using qw::units::RelativeHumidity;
using qw::units::SpeedMeasurement;
using qw::units::SpeedMeasurementTimeStamp;
using qw::weather::dewPoint;
using qw::weather::kInterval10m;
using qw::weather::kInterval2m;
using qw::weather::WindspeedHistory;
using std::cout;
using std::endl;
using std::get;
using std::holds_alternative;
using std::ifstream;
using std::max;
using std::min;
using std::ofstream;
using std::string;
using std::string_view;
using std::chrono::system_clock;
using std::chrono::time_point;
using std::chrono::utc_clock;

int main(int argc, char* argv[]) {
  string temperature;
  string humidity;
  float ctemp, pressure, hum, ftemp, sht44temp, lps22temp;
  string time_string;
  std::expected<uint8_t, int> x_whoami;
  std::expected<uint32_t, int> x_serial_number;
  std::expected<string, SdBusError> service_state;
  std::expected<uint32_t, SdBusError> service_pid;
  int error;
  int c;
  bool in_systemd = false;
  bool reporting_enabled =
      false;  // We are debugging so don't send the data to weather underground
  Ads1015Config value;
  WindspeedHistory ws_history;

  /*
    * If we have started from systemd then we always use
    * LOGGER_MODE_JOURNAL.
    */
  SdUnit sd_qw_unit(systemd_destination, systemd_quietwind_service_path,
                    systemd_unit_interface);
  SdServiceUnit sd_qw_service_unit(systemd_destination,
                                   systemd_quietwind_service_path,
                                   systemd_service_interface);

  service_state = sd_qw_unit.getSubState();
  if (service_state.has_value() != true) {
    logger.log(LOGGER_ERR, "Can't get substate of quietwind weather service");
    exit(1);
  }

  service_pid = sd_qw_service_unit.getMainPID();
  if (service_pid.has_value() != true) {
    logger.log(LOGGER_INFO, "Can't get main pid of quietwind weather service");
    exit(1);
  }

  if (service_state == "running" && service_pid == getpid()) {
    in_systemd = true;
  }

  if (in_systemd == true) {
    logger.setMode(LOGGER_MODE_JOURNAL);
    logger.log(LOGGER_INFO, "Logging in Journal Mode");
  }
  /*
   * Determine the logging mode from parameters
   */
  while ((c = getopt(argc, argv, "l:")) != -1) {
    switch (c) {
      case 'l':
        /*
         * If we are running from systemd then the
         * mode will already have been set to LOGGER_MODE_JOURNAL.
         * We ignore any command line option.
         */
        if (in_systemd == true) {
          break;
        }
        string value = optarg;
        size_t pos;
        /*
         * Check if it begins with file and has a colon
         */
        if (((pos = value.find(":")) != string::npos) &&
            (value.substr(0, args_log_mode_file.length()) ==
             args_log_mode_file)) {
          /*
           * It has a colon so see if it is just "file:"
           */
          if (value == (string(args_log_mode_file) + ":")) {
            logger.setMode(LOGGER_MODE_FILE);
            logger.log(LOG_INFO, "Logging in File Mode to cout");
            break;
          } else {
            /*
             * We know it has "file:<stuff>".
             * So erase the "file:" and we are left with the path
             */
            value.erase(0, pos + 1);
            std::filesystem::path fpath = value;
            if (std::filesystem::exists(fpath) == false) {
              std::filesystem::create_directories(fpath.parent_path());
              std::ofstream log_stream(fpath.string());
              if (log_stream.is_open() == true) {
                log_stream.close();
              }
            }
            logger.setMode(LOGGER_MODE_FILE, fpath);
            logger.log(LOGGER_INFO,
                       format("Logging to file: {}", fpath.c_str()));
            break;
          }
        } else {
          /*
           * No colon so check if it is exactly "file"
           */
          if (value == args_log_mode_file) {
            logger.setMode(LOGGER_MODE_FILE);
            logger.log(LOGGER_INFO, "Logging in File Mode to cout");
            break;
          }
        }
        if (value == "none") {
          logger.setMode(LOGGER_MODE_NOLOGGING);
          logger.log(LOGGER_INFO, "No Logging");
          break;
        }
    }
  }

  /*
   * Load the configuration file
   */
  WeatherStationConfig ws_config(weather_station_config);
  Json::Value json_config;
  ws_config.getRoot(json_config);

  string software_version =
      json_config["Software"]["Version"]["Major"].asString() + "." +
      json_config["Software"]["Version"]["Minor"].asString() + "." +
      json_config["Software"]["Version"]["Patchlevel"].asString();

  logger.log(LOGGER_INFO, format("Software version {}", software_version));
  logger.log(LOGGER_INFO, "Checking Hardware");
  logger.log(LOGGER_INFO,
             format("Model: {}", json_config["Hardware"]["Model"].asString()));

  I2cBus i2c_bus =
      I2cBus(json_config["Hardware"]["I2c"]["Bus"]["name"].asString());
  if (i2c_bus.status() != qw::devices::I2CBUS_STATUS_OK) {
    logger.log(LOGGER_ERR, "Initialization of I2C bus failed");
    if (in_systemd == true) {
      sleep(10);  // Give the daemon a chance to register the log message
      sd_qw_unit.stop("replace");
      pause();
    }
    exit(1);
  }

  Lps22 lps22(i2c_bus, kLps22hbI2cPrimaryAddress);

  error = lps22.init();
  if (error != 0) {
    logger.log(LOGGER_ERR, "Initialization of lps22hb Failed");
    if (in_systemd == true) {
      sleep(10);  // Give the daemon a chance to register the log message
      sd_qw_unit.stop("replace");
      pause();
    }
    exit(1);
  }

  x_whoami = lps22.whoAmI();
  if (x_whoami.has_value() != true) {
    logger.log(LOGGER_ERR, "Couldn't get Who am I value for lps22hb");
    if (in_systemd == true) {
      sleep(10);  // Give the daemon a chance to register the log message
      sd_qw_unit.stop("replace");
      pause();
    }
    exit(1);
  }
  logger.log(LOGGER_INFO,
             format("LPS22HB who am I Value: {:#X}", x_whoami.value()));

  /*
   * The sht4x device is connected to I2c bus 1 at the primary address
   */
  I2cSht4x sht4x(i2c_bus, kSht4xI2cPrimaryAddress);

  error = sht4x.softReset();
  if (error != 0) {
    logger.log(LOGGER_ERR, "CHT4X reset failed");
    if (in_systemd == true) {
      sleep(10);  // Give the daemon a chance to register the log message
      sd_qw_unit.stop("replace");
      pause();
    }
    exit(1);
  }
  x_serial_number = sht4x.getSerialNumber();
  if (x_serial_number.has_value() == false) {
    logger.log(LOGGER_ERR, "Getting SHT44 Serial Number failed");
    if (in_systemd == true) {
      sleep(10);  // Give the daemon a chance to register the log message
      sd_qw_unit.stop("replace");
      pause();
    }
    exit(1);
  }
  logger.log(LOGGER_INFO,
             format("SHT44 Serial Number: {}", x_serial_number.value()));

  /*
   * Add the ads device
   */
  I2cAds1015 ads1015(i2c_bus, kAds1015I2cPrimaryAddress);
  /*
   * Check if we can get the configuration register
   */
  expected<Ads1015Config, int> ads_result = ads1015.inspectConfigRegister();
  if (ads_result.has_value() == false) {
    logger.log(LOGGER_ERR, "Getting ADS1015 Configuration Register");
    if (in_systemd == true) {
      sleep(10);  // Give the daemon a chance to register the log message
      sd_qw_unit.stop("replace");
      pause();
    }
    exit(1);
  }
  logger.log(LOGGER_INFO, "Ads 1015 Successfully read configuration register");
  value = ads_result.value();
  logger.log(
      LOGGER_INFO,
      format(
          "\tOs: {}\n\tMux: {}\n\tPga: {}\n\tMode: {}\n\tDr: {}\n\tCompMode: "
          "{}\n\tCompPol: {}\n\tCompLatch: {}\n\tCompQueue: {}\n",
          static_cast<uint8_t>(value.fields.os),
          static_cast<uint8_t>(value.fields.mux),
          static_cast<uint8_t>(value.fields.pga),
          static_cast<uint8_t>(value.fields.mode),
          static_cast<uint8_t>(value.fields.dr),
          static_cast<uint8_t>(value.fields.comp_mode),
          static_cast<uint8_t>(value.fields.comp_pol),
          static_cast<uint8_t>(value.fields.comp_latch),
          static_cast<uint8_t>(value.fields.comp_queue)));

  /*
   * Since the ADC is available define an annometer
   */
  AnomometerAdafruit anomometer(ads1015, ADS1015_MUX_AIN0_GND);
  expected<SpeedMeasurement, int> wind_speed_measurement =
      anomometer.getMeasurement();
  if (wind_speed_measurement.has_value() == false) {
    logger.log(LOGGER_ERR, "Unable to read anomometer speed");
    if (in_systemd == true) {
      sleep(10);  // Give the daemon a chance to register the log message
      sd_qw_unit.stop("replace");
      pause();
    }
    exit(1);
  }

  /*
   * Starting to gather data
   */
  logger.log(LOGGER_INFO, "Starting");

  /*
   * Set the data_gather_interval
   */
  int data_gathering_interval =
      json_config["Configuration"]["data_gathering_interval"].asInt();
  /*
   * Get the Weather Underground configuration
   */
  WeatherUndergroundConfig wu_config(
      json_config["WeatherUndegroundFile"].asString());
  if (wu_config.exists() == false) {
    logger.log(LOGGER_INFO, "Can't get Weather Underground configuration info");
    exit(1);
  }

  Json::Value wu_json_config;
  if (wu_config.getRoot(wu_json_config) == false) {
    logger.log(LOGGER_INFO,
               format("Unable to parse Weather Underground config file: {}",
                      json_config["WeatherUndegroundFile"].asString()));
    exit(1);
  }

  if (wu_json_config.isMember("pwu_name") == false ||
      wu_json_config.isMember("pwu_password") == false) {
    logger.log(LOGGER_INFO,
               "Improperly formatted Weather Underground config file no "
               "authentication info");
  }
  string pwu_name = wu_json_config["pwu_name"].asString();
  string pwu_password = wu_json_config["pwu_password"].asString();

  WeatherUnderground* wu = new WeatherUnderground(pwu_name, pwu_password);
  int reporting_loop_interval = wu_default_report_interval;
  if (wu_json_config.isMember("report_interval") == true) {
    reporting_loop_interval = min(
        max(wu_report_interval_min, wu_json_config["report_interval"].asInt()),
        wu_report_interval_max);
  }
  /*
   * Initializing last reporting time to 2 reporting loops prior to now so a report is
   * sent on first pass.
   */
  auto reporting_interval = std::chrono::milliseconds(reporting_loop_interval);
  auto last_report_time = system_clock::now() - (reporting_interval * 2);
  /*
   * Setup inotify to get notified when config file changes during poll
   */
  int inotify_fd = inotify_init();
  int inotify_ws_watch_d =
      inotify_add_watch(inotify_fd, weather_station_config.c_str(), IN_MODIFY);
  int inotify_wu_watch_d = inotify_add_watch(
      inotify_fd, json_config["WeatherUndegroundFile"].asString().c_str(),
      IN_MODIFY);
  pollfd fds[1];
  fds[0].fd = inotify_fd;
  fds[0].events = POLLIN;

  while (true) {
    if (pwu_name == "" || pwu_password == "") {
      /*
       * If there is no Weather Underground username and password
       * then don't gather any data.
       */
      logger.log(LOGGER_INFO, "Invalid Weather Underground Authentication");
    } else {
      /*
       * Gather the data. 
       * get the current time.
       */
      auto now_time = system_clock::now();
      logger.log(LOGGER_INFO, format("{:%F %T}", now_time));

      /*
       * We need wind information on each data gathering pass
       */
      auto x_anomometer = anomometer.getMeasurement();

      /*
       * If we successfully got a anomometer measurement add it to the history
       */
      if (x_anomometer.has_value()) {
        ws_history.add(x_anomometer.value());
      }

      if ((now_time - last_report_time) >=
          std::chrono::milliseconds(reporting_loop_interval)) {
        /*
         * We only need this information when we are going to make a report so get it now.
         */
        auto x_sht4x_temp = sht4x.getTemperatureMeasurement();

        auto x_sht4x_humidity = sht4x.getRelativeHumidityMeasurement();

        auto x_lps22_temp = lps22.getTemperatureMeasurement();

        auto x_lps22_pressure = lps22.getPressureMeasurement();
        /*
         * Put the raw data into the wu data
         */
        wu->setVarData("action", "updateraw");
        // time_point<utc_clock> utc_time = utc_clock::now();
        wu->setVarData("dateutc", "now");
        /*
         * Weather Underground wants fahrenheit
         */
        if (x_sht4x_temp.has_value()) {
          /*
           * The SHT4x is supposed to be more accurate so use it
           */
          Fahrenheit tempf = x_sht4x_temp.value().value();
          wu->setVarData("tempf", tempf.value());
          Fahrenheit temp2f = x_lps22_temp.value().value();
          wu->setVarData("temp2f", temp2f.value());
        }

        if (x_sht4x_humidity.has_value()) {
          RelativeHumidity humidity =
              x_sht4x_humidity.value().relativeHumidityValue();
          wu->setVarData("humidity", humidity.value());
        }

        /*
         * If there are valid temperature and relative humidity then add a dewpoint
         */
        if (x_sht4x_temp.has_value() && x_sht4x_humidity.has_value()) {
          Celsius tempc = x_sht4x_temp.value().value();
          RelativeHumidity humidity = x_sht4x_humidity.value().value();
          Celsius dewptc = dewPoint(tempc, humidity);
          Fahrenheit dewptf = dewptc;
          wu->setVarData("dewptf", dewptf.value());
        }

        /*
         * Weather Underground wants inches mercury
         */
        if (x_lps22_pressure.has_value()) {
          InchesMercury pressure = x_lps22_pressure.value().value();
          wu->setVarData("baromin", pressure.value());
        }

        /*
         * Weather Underground wants speed in mph
         */
        if (x_anomometer.has_value()) {
          MilesPerHour wind_mph = x_anomometer.value().value();
          wu->setVarData("windspeedmph", wind_mph.value());
        }
        if (ws_history.countOverPeriod(kInterval2m) > 0) {
          expected<MilesPerHour, int> ave = ws_history.average(kInterval2m);
          if (ave.has_value() == true) {
            wu->setVarData("windspdmph_avg2m", ave.value().value());
          }
          expected<SpeedMeasurement, int> gust = ws_history.gust(kInterval2m);
          if (gust.has_value() == true) {
            MilesPerHour mph = gust.value().value();
            wu->setVarData("windgustmph", mph.value());
          }
        }
        if (ws_history.countOverPeriod(kInterval10m) > 0) {
          expected<SpeedMeasurement, int> gust = ws_history.gust(kInterval10m);
          if (gust.has_value() == true) {
            MilesPerHour mph = gust.value().value();
            wu->setVarData("windgustmph_10m", mph.value());
          }
        }

        /*
         * debug to check out the string
         */
        string http_request = wu->buildHttpRequest();
        logger.log(LOGGER_INFO, http_request);
        /*
         * When we are deugging we may not want to actually send the
         * data. So, only send is reporting enabled is on
         */
        if (reporting_enabled == true) {
          auto errval = wu->sendData();
          if (errval.has_value() == false) {
            logger.log(LOGGER_ERR,
                       format("Send Data Failed: {}", errval.error()));
          }

          string response = wu->getHttpResponse();

          logger.log(LOGGER_INFO, response);
        }
        wu->reset();
      }
    }

    int poll_cnt = poll(fds, 1, data_gathering_interval);
    /*
     * If poll_cnt is zero it means the configuration file was
     * not updated and we can just cycle through and gather
     * another set of data.
     */
    if (poll_cnt != 0 || (pwu_name == "" || pwu_password == "")) {
      delete wu;
      /*
       * If we got here it means the configuration file was
       * changed. Or the authentication was invalid. So, we have to
       * get a new username and password and then gather more data.
       */
      if ((wu_config.getRoot(wu_json_config) == false) ||
          (wu_json_config.isMember("pwu_name") == false) ||
          (wu_json_config.isMember("pwu_password") == false)) {
        logger.log(LOGGER_INFO,
                   "Unable to parse Weather Underground config file");
      } else {
        pwu_name = wu_json_config["pwu_name"].asString();
        pwu_password = wu_json_config["pwu_password"].asString();
        wu = new WeatherUnderground(pwu_name, pwu_password);
      }
      reporting_loop_interval = wu_default_report_interval;
      if (wu_json_config.isMember("report_interval") == true) {
        reporting_loop_interval = min(
            max(wu_report_interval_min, json_config["report_interval"].asInt()),
            wu_report_interval_max);
      }
    }
  }
}
