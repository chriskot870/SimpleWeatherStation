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
#include "qw/devices/include/ecowitt_ln90lp.h"
#include "qw/locking/include/locking_file.h"
#include "qw/logger/include/logger.h"
#include "qw/systemd/include/sd_service_unit.h"
#include "qw/systemd/include/sd_unit.h"
#include "qw/systemd/include/systemd.h"
#include "qw/units/distance/include/distance.h"
#include "qw/units/distance/include/millimeter.h"
#include "qw/units/humidity/include/relative_humidity.h"
#include "qw/units/include/unit_measurement.h"
#include "qw/units/light/include/light.h"
#include "qw/units/light/include/lux.h"
#include "qw/units/pressure/include/inches_mercury.h"
#include "qw/units/speed/include/miles_per_hour.h"
#include "qw/units/temperature/include/fahrenheit.h"
#include "qw/weather/include/dewpoint.h"
#include "qw/weather/include/weather_device.h"

using fmt::format;
using qw::devices::ADS1015_MUX_AIN0_GND;
using qw::devices::Ads1015Config;
using qw::devices::AnomometerAdafruit;
using qw::devices::EcowittLn90lp;
using qw::devices::I2cAds1015;
using qw::devices::I2cBus;
using qw::devices::I2cSht4x;
using qw::devices::kAds1015I2cPrimaryAddress;
using qw::devices::kEwLn90lpBaudRates;
using qw::devices::kEwLn90lpRtuDevice;
using qw::devices::kEwLn90lpRtuDeviceId;
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
using qw::units::Degrees;
using qw::units::Direction;
using qw::units::Distance;
using qw::units::Fahrenheit;
using qw::units::InchesMercury;
using qw::units::Kelvin;
using qw::units::kHistoryInterval10m;
using qw::units::kHistoryInterval2m;
using qw::units::KilometersPerHour;
using qw::units::Light;
using qw::units::Lux;
using qw::units::MeasurementHistory;
using qw::units::MilesPerHour;
using qw::units::Millibar;
using qw::units::Millimeter;
using qw::units::Pressure;
using qw::units::RelativeHumidity;
using qw::units::Speed;
using qw::units::Temperature;
using qw::units::UnitMeasurement;
using qw::units::Uvi;
using qw::weather::dewPoint;
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
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
using std::chrono::time_point;
using std::chrono::utc_clock;

expected<bool, SdBusError> isRunningInSystemd() {
  expected<string, SdBusError> service_state;
  std::expected<uint32_t, SdBusError> service_pid;

  SdUnit sd_qw_unit(systemd_destination, systemd_quietwind_service_path,
                    systemd_unit_interface);
  SdServiceUnit sd_qw_service_unit(systemd_destination,
                                   systemd_quietwind_service_path,
                                   systemd_service_interface);

  service_state = sd_qw_unit.getSubState();
  if (service_state.has_value() != true) {
    return unexpected(service_state.error());
  }

  service_pid = sd_qw_service_unit.getMainPID();
  if (service_pid.has_value() != true) {
    return unexpected(service_pid.error());
  }

  if (service_state == "running" && service_pid == getpid()) {
    return true;
  }

  return false;
}

void parseCommandLine(int argc, char** argv, bool parse_log) {
  int c;
  /*
   * Determine the logging mode from parameters
   */
  while ((c = getopt(argc, argv, "l:")) != -1) {
    switch (c) {
      case 'l':
        /*
         * If we are running from systemd then the
         * mode will already have been set to LOGGER_MODE_JOURNAL.
         * In that case parse_log will be false.
         * We ignore processeing the log option when parse_log is false.
         */
        if (parse_log == false) {
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
}

void terminate(bool in_systemd) {
  if (in_systemd == true) {
    SdUnit sd_qw_unit(systemd_destination, systemd_quietwind_service_path,
                      systemd_unit_interface);
    sleep(10);  // Give the daemon a chance to register the log message
    sd_qw_unit.stop("replace");
    pause();
  }
  exit(1);
}

/*
 * Define the various weather devices
 */
qw::weather::WeatherDevice<Temperature> getEcowittThermometer(
    EcowittLn90lp& ecowitt) {  // NOLINT
  return qw::weather::WeatherDevice<Temperature>(
      [&ecowitt]() -> expected<UnitMeasurement<qw::units::Temperature>, int> {
        return ecowitt.getTemperature();
      },
      [&ecowitt]() -> milliseconds {
        return ecowitt.getTemperatureValidInterval();
      },
      [&ecowitt](milliseconds interval) -> void {
        return ecowitt.setTemperatureValidInterval(interval);
      });
}

qw::weather::WeatherDevice<RelativeHumidity> getEcowittHygrometer(
    EcowittLn90lp& ecowitt) {  // NOLINT
  return qw::weather::WeatherDevice<RelativeHumidity>(
      [&ecowitt]()
          -> expected<UnitMeasurement<qw::units::RelativeHumidity>, int> {
        return ecowitt.getRelativeHumidity();
      },
      [&ecowitt]() -> milliseconds {
        return ecowitt.getRelativeHumidityValidInterval();
      },
      [&ecowitt](milliseconds interval) -> void {
        return ecowitt.setRelativeHumidityValidInterval(interval);
      });
}

qw::weather::WeatherDevice<Pressure> getEcowittBarometer(
    EcowittLn90lp& ecowitt) {  // NOLINT
  return qw::weather::WeatherDevice<Pressure>(
      [&ecowitt]() -> expected<UnitMeasurement<qw::units::Pressure>, int> {
        return ecowitt.getPressure();
      },
      [&ecowitt]() -> milliseconds {
        return ecowitt.getPressureValidInterval();
      },
      [&ecowitt](milliseconds interval) -> void {
        return ecowitt.setPressureValidInterval(interval);
      });
}

qw::weather::WeatherDevice<Speed> getEcowittAnemometer(
    EcowittLn90lp& ecowitt) {  // NOLINT
  return qw::weather::WeatherDevice<Speed>(
      [&ecowitt]() -> expected<UnitMeasurement<qw::units::Speed>, int> {
        return ecowitt.getWindspeed();
      },
      [&ecowitt]() -> milliseconds {
        return ecowitt.getWindSpeedValidInterval();
      },
      [&ecowitt](milliseconds interval) -> void {
        return ecowitt.setWindSpeedValidInterval(interval);
      });
}

qw::weather::WeatherDevice<Direction> getEcowittWindVane(
    EcowittLn90lp& ecowitt) {  // NOLINT
  return qw::weather::WeatherDevice<Direction>(
      [&ecowitt]() -> expected<UnitMeasurement<qw::units::Direction>, int> {
        return ecowitt.getWindDirection();
      },
      [&ecowitt]() -> milliseconds {
        return ecowitt.getWindDirectionValidInterval();
      },
      [&ecowitt](milliseconds interval) -> void {
        return ecowitt.setWindDirectionValidInterval(interval);
      });
}

qw::weather::WeatherDevice<Uvi> getEcowittUvMeter(
    EcowittLn90lp& ecowitt) {  // NOLINT
  return qw::weather::WeatherDevice<Uvi>(
      [&ecowitt]() -> expected<UnitMeasurement<qw::units::Uvi>, int> {
        return ecowitt.getUvi();
      },
      [&ecowitt]() -> milliseconds { return ecowitt.getUviValidInterval(); },
      [&ecowitt](milliseconds interval) -> void {
        return ecowitt.setUviValidInterval(interval);
      });
}

qw::weather::WeatherDevice<Light> getEcowittPhotometer(
    EcowittLn90lp& ecowitt) {  // NOLINT
  return qw::weather::WeatherDevice<Light>(
      [&ecowitt]() -> expected<UnitMeasurement<qw::units::Light>, int> {
        return ecowitt.getLight();
      },
      [&ecowitt]() -> milliseconds { return ecowitt.getLightValidInterval(); },
      [&ecowitt](milliseconds interval) -> void {
        return ecowitt.setLightValidInterval(interval);
      });
}

qw::weather::WeatherDevice<Distance> getEcowittRainGauge(
    EcowittLn90lp& ecowitt) {  // NOLINT
  return qw::weather::WeatherDevice<Distance>(
      [&ecowitt]() -> expected<UnitMeasurement<qw::units::Distance>, int> {
        return ecowitt.getRainFall();
      },
      [&ecowitt]() -> milliseconds {
        return ecowitt.getRainFallValidInterval();
      },
      [&ecowitt](milliseconds interval) -> void {
        return ecowitt.setRainFallValidInterval(interval);
      });
}

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
  /*
  WindspeedHistory ws_history;
  WindDirectionHistory wd_history;
   */
  MeasurementHistory<Speed> ws_history(
      seconds(600));  // Keep 10 minutes of history
  MeasurementHistory<Direction> wd_history(
      seconds(600));  // Keep 10 minutes of history
  MeasurementHistory<Distance> rf_history(
      seconds(3600));  // Store 1 hour worth of rain?

  /*
    * If we have started from systemd then we always use
    * LOGGER_MODE_JOURNAL.
    */
  SdUnit sd_qw_unit(systemd_destination, systemd_quietwind_service_path,
                    systemd_unit_interface);

  expected<bool, SdBusError> isParentSystemd = isRunningInSystemd();

  if (isParentSystemd.has_value() != true) {
    logger.log(LOG_CRIT, format("{} : {}", isParentSystemd.error().code,
                                *isParentSystemd.error().message));
    exit(1);
  }

  in_systemd == isParentSystemd.value();

  if (in_systemd == true) {
    logger.setMode(LOGGER_MODE_JOURNAL);
    logger.log(LOGGER_INFO, "Logging in Journal Mode");
    // Since we are logging in Journal Mode we don't need to parse -l option
    // We still need to process any other command line options
    parseCommandLine(argc, argv, false);
  } else {
    parseCommandLine(argc, argv, true);
  }

  /*
   * Load the configuration file
   */
  WeatherStationConfig ws_config(weather_station_config.data());
  expected<bool, int> load = ws_config.load();
  if (load.has_value() != true) {
    logger.log(LOG_CRIT, format("Unable to load configuration file {}: {}",
                                weather_station_config.data(), load.error()));
    terminate(in_systemd);
  }

  expected<string, int> version = ws_config.softwareVersion();
  if (load.has_value() != true) {
    logger.log(LOGGER_INFO, "Unable to determine Software Version");
  } else {
    logger.log(LOGGER_INFO, format("Software version {}", version.value()));
  }
  logger.log(LOGGER_INFO, "Checking Hardware");

  expected<string, int> model = ws_config.model();
  if (model.has_value() != true) {
    logger.log(LOGGER_INFO, "Unable to determine Model");
  } else {
    logger.log(LOGGER_INFO, format("Model: {}", model.value()));
  }

  expected<string, int> busname = ws_config.i2cBusName();
  if (busname.has_value() != true) {
    logger.log(LOG_CRIT,
               format("Unable to determine i2c bus name: {}", busname.error()));
    terminate(in_systemd);
  }
  I2cBus i2c_bus = I2cBus(busname.value());
  if (i2c_bus.status() != qw::devices::I2CBUS_STATUS_OK) {
    logger.log(LOGGER_ERR, "Initialization of I2C bus failed");
    terminate(in_systemd);
  }

  string serial_port = kEwLn90lpRtuDevice.data();
  expected<string, int> x_serial_port = ws_config.getModbusSerialPort();
  if (x_serial_port.has_value() == true) {
    serial_port = x_serial_port.value();  // /dev/ttyS0
  }
  EcowittLn90lp ecowitt(serial_port);

  /*
   * Initialize to the fastest speed
   * May want to get this from configuration file
   */
  uint32_t baud_rate = kEwLn90lpBaudRates[kEwLn90lpBaudRates.size() - 1];
  uint8_t ecowitt_address = kEwLn90lpRtuDeviceId;
  expected<uint32_t, int> x_baud_rate = ws_config.getEcowittLn90lpBaudRate();
  if (x_baud_rate.has_value() == true) {
    baud_rate = x_baud_rate.value();
  }
  expected<uint8_t, int> x_ecowitt_address =
      ws_config.getEcowittLn90lpAddress();
  if (x_ecowitt_address.has_value() == true) {
    ecowitt_address = x_ecowitt_address.value();
  }
  if (ecowitt.initialize(baud_rate, ecowitt_address) != true) {
    logger.log(LOG_CRIT, "Couldn't find Ecowitt LN90lp device");
    logger.log(LOG_CRIT, "Can not continue");
    terminate(in_systemd);
  }

  expected<uint16_t, int> ecowitt_expect_id = ecowitt.getDeviceId();
  if (ecowitt_expect_id.has_value() != true) {
    logger.log(LOG_CRIT, "Couldn't get Ecowitt device id");
    terminate(in_systemd);
  }
  uint16_t ecowitt_device_id = ecowitt_expect_id.value();

  if (ecowitt_device_id != kEwLn90lpRtuDeviceId) {
    logger.log(LOG_CRIT, "Incorrect Device ID for Ecowitt LN90lp");
    terminate(in_systemd);
  }

  /*
   * Define all the weather devices using the Ecowitt LN90lp device
   */
  qw::weather::WeatherDevice<Temperature> thermometer_1 =
      getEcowittThermometer(ecowitt);
  qw::weather::WeatherDevice<RelativeHumidity> hygrometer_1 =
      getEcowittHygrometer(ecowitt);
  qw::weather::WeatherDevice<Pressure> barometer_1 =
      getEcowittBarometer(ecowitt);
  qw::weather::WeatherDevice<Speed> anemometer_1 =
      getEcowittAnemometer(ecowitt);
  qw::weather::WeatherDevice<Direction> wind_vane_1 =
      getEcowittWindVane(ecowitt);
  qw::weather::WeatherDevice<Uvi> uv_meter_1 = getEcowittUvMeter(ecowitt);
  qw::weather::WeatherDevice<Light> photometer_1 =
      getEcowittPhotometer(ecowitt);
  qw::weather::WeatherDevice<Distance> rain_gauge_1 =
      getEcowittRainGauge(ecowitt);

  /*
   * Starting to gather data
   */
  logger.log(LOGGER_INFO, "Starting");

  /*
   * Get data_gather_interval
   */
  milliseconds data_gathering_interval;
  expected<milliseconds, int> get_data_interval =
      ws_config.getDataAcquisitionInterval();
  if (get_data_interval.has_value() != true) {
    data_gathering_interval = ws_data_gathering_interval_default;
  } else {
    data_gathering_interval =
        milliseconds(min(max(ws_data_gathering_interval_min.count(),
                             get_data_interval.value().count()),
                         ws_data_gathering_interval_max.count()));
  }
  logger.log(LOG_INFO,
             format("Set data gathering interval to {} milliseconds",
                    data_gathering_interval.count()));

  expected<string, int> get_wu_pwu_name = ws_config.getWuPwuName();
  string pwu_name = get_wu_pwu_name.value();
  expected<string, int> get_wu_pwu_password = ws_config.getWuPwuPassword();
  string pwu_password = get_wu_pwu_password.value();

  WeatherUnderground* wu = new WeatherUnderground(pwu_name, pwu_password);
  milliseconds reporting_loop_interval;
  expected<milliseconds, int> get_report_interval =
      ws_config.getWuReportInterval();
  if (get_report_interval.has_value() != true) {
    reporting_loop_interval = wu_default_report_interval;
  } else {
    reporting_loop_interval =
        milliseconds(min(max(wu_report_interval_min.count(),
                             get_report_interval.value().count()),
                         wu_report_interval_max.count()));
  }

  expected<bool, int> get_report_enabled = ws_config.getWuReportEnabled();
  if (get_report_enabled.has_value() != true) {
    logger.log(LOG_INFO, format("Couldn't determine if WU reporting is "
                                "enabled. Setting to false: {}",
                                get_report_enabled.error()));
    reporting_enabled = false;
  } else {
    reporting_enabled = get_report_enabled.value();
  }
  /*
   * Initializing last reporting time to 2 reporting loops prior to now so a report is
   * sent on first pass.
   */
  time_point last_report_time =
      system_clock::now() - (reporting_loop_interval * 2);
  /*
   * Setup inotify to get notified when config file changes during poll
   */
  int inotify_fd = inotify_init();
  expected<string, int> get_fname = ws_config.configVariablesFileName();
  int inotify_wu_watch_d =
      inotify_add_watch(inotify_fd, get_fname.value().c_str(), IN_MODIFY);
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
      logger.log(LOGGER_DEBUG, format("{:%F %T} Gathering Data", now_time));

      /*
       * We need wind information on each data gathering pass
       */
      auto x_wind_speed_measurement = anemometer_1.getData();

      /*
       * If we successfully got an anemometer measurement add it to the history
       */
      if (x_wind_speed_measurement.has_value()) {
        ws_history.add(x_wind_speed_measurement.value());
      }

      auto x_wind_dir_measurement = wind_vane_1.getData();

      /*
       * If we successfully got a direction from the wind vane add it to history
       */
      if (x_wind_dir_measurement.has_value()) {
        wd_history.add(x_wind_dir_measurement.value());
      }

      auto x_rain_fall_measurement = rain_gauge_1.getData();

      /*
       * If we successfully got a reading from rainfall add it to the history
       */
      if (x_rain_fall_measurement.has_value()) {
        /*
         * We only add if we got a value
         */
        Millimeter rf = x_rain_fall_measurement.value().measurement();
        if (rf >= Millimeter(1)) {
          rf_history.add(x_rain_fall_measurement.value());
        }
      }
      /*
       * Now see if it's time to report
       */
      if ((now_time - last_report_time) >= reporting_loop_interval) {
        if (reporting_enabled == true) {
          logger.log(LOGGER_INFO,
                     format("{:%F %T} Reporting Enabled - Sending Data to WU",
                            now_time));
        } else {
          logger.log(
              LOGGER_INFO,
              format("{:%F %T} Reporting Disabled - Not Sending Data to WU",
                     now_time));
        }
        /*
         * We only need this information when we are going to make a report so get it now.
         * We are making a report so set the new report time
         */
        last_report_time = now_time;
        // gather all temperatures and put them on temp_list
        std::vector<UnitMeasurement<Temperature>> temp_list;
        auto x_temp = thermometer_1.getData();
        if (x_temp.has_value()) {
          temp_list.push_back(x_temp.value());
        }
        // If we have any temps add them
        if (temp_list.empty() != true) {
          wu->addTemperatureMeasurements(temp_list);
        }

        auto x_humidity = hygrometer_1.getData();
        if (x_humidity.has_value()) {
          wu->addRelativeHumidityMeasurement(x_humidity.value());
        }

        auto x_pressure = barometer_1.getData();
        if (x_pressure.has_value()) {
          wu->addPressureMeasurement(x_pressure.value());
        }

        expected<Fahrenheit, int> dewptf;
        if (x_temp.has_value() && x_humidity.has_value()) {
          dewptf = dewPoint(x_temp.value().measurement(),
                            x_humidity.value().measurement());
          if (dewptf.has_value()) {
            wu->setVarData("dewptf", dewptf.value().toString());
          }
        } else {
          dewptf = unexpected(ENODATA);
        }

        auto x_uvi = uv_meter_1.getData();
        if (x_uvi.has_value()) {
          wu->addUviMeasurement(x_uvi.value());
        }

        auto x_light = photometer_1.getData();
        if (x_light.has_value()) {
          wu->addLightMeasurement(x_light.value());
        }

        wu->addWindMeasurement(ws_history, wd_history);

        /*
         * debug to check out the string
         */
        expected<string, int> http_request = wu->buildHttpRequest();
        if (http_request.has_value() != true) {
          logger.log(LOG_INFO, "Couldn't build HTTP request for logging");
        } else {
          logger.log(LOGGER_INFO, http_request.value());
        }
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

    int poll_cnt = poll(fds, 1, data_gathering_interval.count());
    /*
     * If poll_cnt is zero it means the configuration file was
     * not updated and we can just cycle through and gather
     * another set of data.
     */
    if (poll_cnt != 0 || (pwu_name == "" || pwu_password == "")) {
      /*
       * If we got here it means the configuration file was
       * changed. Or the authentication was invalid. So, we have to
       * get a new username and password and then gather more data.
       */
      ws_config.load();
      expected<string, int> get_pwu_name = ws_config.getWuPwuName();
      pwu_name = get_pwu_name.value();
      expected<string, int> get_pwu_passwd = ws_config.getWuPwuPassword();
      pwu_password = get_pwu_passwd.value();
      expected<milliseconds, int> get_report_interval =
          ws_config.getWuReportInterval();
      if (get_report_interval.has_value() != true) {
        reporting_loop_interval = get_report_interval.value();
      } else {
        reporting_loop_interval =
            milliseconds(min(max(wu_report_interval_min.count(),
                                 get_report_interval.value().count()),
                             wu_report_interval_max.count()));
      }
      expected<bool, int> get_report_enabled = ws_config.getWuReportEnabled();
      if (get_report_enabled.has_value() != true) {
        logger.log(LOG_INFO, format("Couldn't determine if WU reporting is "
                                    "enabled. Setting to false: {}",
                                    get_report_enabled.error()));
        reporting_enabled = false;
      } else {
        reporting_enabled = get_report_enabled.value();
      }
      expected<milliseconds, int> get_data_interval =
          ws_config.getDataAcquisitionInterval();
      if (get_data_interval.has_value() != true) {
        data_gathering_interval = ws_data_gathering_interval_default;
        logger.log(
            LOG_INFO,
            format("Setting data gathering interval to default {} milliseconds",
                   data_gathering_interval.count()));
      } else {
        data_gathering_interval =
            milliseconds(min(max(ws_data_gathering_interval_min.count(),
                                 get_data_interval.value().count()),
                             ws_data_gathering_interval_max.count()));
      }
    }
  }
}
