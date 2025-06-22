/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * The main program to show the data.
 * Right now it's pretty simple
 */

#include "locking_file.h"
#include "logger.h"
#include "systemd.h"
#include "weather_station.h"
#include "weather_station_config.h"
#include "systemd_quietwind_weather.h"
#include "sd_unit_obj.h"
#include "sd_service_unit_obj.h"

#include "include/lps22.h"
#include "include/sht4x.h"

#include "fmt/chrono.h"
#include "fmt/format.h"
#include "include/weather_underground.h"

#include "celsius.h"
#include "fahrenheit.h"
#include "inches_mercury.h"
#include "kelvin.h"
#include "millibar.h"
#include "relative_humidity.h"

#include "dewpoint.h"

using fmt::format;
using qw_devices::I2cBus;
using qw_devices::I2cSht4x;
using qw_devices::kLps22hbI2cPrimaryAddress;
using qw_devices::kSht4xI2cPrimaryAddress;
using qw_devices::Lps22;
using qw_units::Celsius;
using qw_units::Fahrenheit;
using qw_units::InchesMercury;
using qw_units::Kelvin;
using qw_units::Millibar;
using std::cout;
using std::endl;
using std::get;
using std::holds_alternative;
using std::ifstream;
using std::ofstream;
using std::string;
using std::chrono::system_clock;
using std::chrono::time_point;
using std::chrono::utc_clock;

extern Logger logger;

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

  /*
    * If we have started from systemd then we always use
    * LOGGER_MODE_JOURNAL.
    */
  SdUnitObj sd_qw_unit(systemd_destination,
                       systemd_quietwind_service_path,
                       systemd_unit_interface);
  SdServiceUnitObj sd_qw_service_unit(systemd_destination,
                       systemd_quietwind_service_path,
                       systemd_service_interface);

  service_state = sd_qw_unit.getSubState();
  if (service_state.has_value() != true) {
    logger.log(LOG_ERR, "Can't get substate of quietwind weather service");
    exit(1);
  }

  service_pid = sd_qw_service_unit.getMainPID();
  if (service_pid.has_value() != true) {
    logger.log(LOG_ERR, "Can't get main pid of quietwind weather service");
    exit(1);
  }

  if (service_state == "running" && service_pid == getpid()) {
    in_systemd = true;
  }
  
  if (in_systemd == true) {
    logger.setMode(LOGGER_MODE_JOURNAL);
    logger.log(LOG_INFO, "Logging in Journal Mode");
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
          if (value == (args_log_mode_file + ":")) {
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
            logger.log(LOG_INFO, format("Logging to file: {}", fpath.c_str()));
            break;
          }
        } else {
          /*
           * No colon so check if it is exactly "file"
           */
          if (value == args_log_mode_file) {
            logger.setMode(LOGGER_MODE_FILE);
            logger.log(LOG_INFO, "Logging in File Mode to cout");
            break;
          }
        }
        if (value == "none") {
          logger.setMode(LOGGER_MODE_NOLOGGING);
          logger.log(LOG_INFO, "No Logging");
          break;
        }
    }
  }

  /*
   * Check the configuration file and initialize values from it
   */
  WeatherStationConfig ws_config(weather_station_config);
  if (ws_config.exists() == false) {
    ws_config.initialize();
  }
  Json::Value json_config;
  ws_config.getRoot(json_config);

  I2cBus i2c_bus = I2cBus(json_config["I2cBus"]["weather_devices"].asString());

  Lps22 lps22(i2c_bus, kLps22hbI2cPrimaryAddress);

  logger.log(LOG_INFO, "Checking Hardware");

  error = lps22.init();
  if (error != 0) {
    logger.log(LOG_ERR, "Initialization of lps22hb Failed");
    if (in_systemd == true) {
      sleep(10); // Give the daemon a chance to register the log message
      sd_qw_unit.Stop("replace");
      pause();
    }
    exit(1);
  }

  x_whoami = lps22.whoami();
  if (x_whoami.has_value() != true) {
    logger.log(LOG_ERR, "Couldn't get Who am I value for lps22hb");
    if (in_systemd == true) {
      sleep(10); // Give the daemon a chance to register the log message
      sd_qw_unit.Stop("replace");
      pause();
    }
    exit(1);
  }
  logger.log(LOG_INFO,
             format("LPS22HB who am I Value: {:#X}", x_whoami.value()));

  /*
   * The sht4x device is connected to I2c bus 1 at the primary address
   */
  I2cSht4x sht4x(i2c_bus, kSht4xI2cPrimaryAddress);

  error = sht4x.softReset();
  if (error != 0) {
    logger.log(LOG_ERR, "CHT4X reset failed");
    if (in_systemd == true) {
      sleep(10); // Give the daemon a chance to register the log message
      sd_qw_unit.Stop("replace");
      pause();
    }
    exit(1);
  }
  x_serial_number = sht4x.getSerialNumber();
  if (x_serial_number.has_value() == false) {
    logger.log(LOG_ERR, "Getting SHT44 Serial Number failed");
    if (in_systemd == true) {
      sleep(10); // Give the daemon a chance to register the log message
      sd_qw_unit.Stop("replace");
      pause();
    }
    exit(1);
  }
  logger.log(LOG_ERR,
             format("SHT44 Serial Number: {}", x_serial_number.value()));

  logger.log(LOG_INFO, "Starting");

  WeatherUnderground* wu = new WeatherUnderground(
      json_config["WeatherUnderground"]["pwu_name"].asString(),
      json_config["WeatherUnderground"]["pwu_password"].asString());
  int reporting_loop_interval =
      min(max(ws_report_interval_min, json_config["WeatherUndergroubnd"]["report_interval"].asInt()),
          ws_report_interval_max);

  /*
   * Setup inotify to get notified when config file changes during poll
   */
  int inotify_fd = inotify_init();
  int watch_fd =
      inotify_add_watch(inotify_fd, weather_station_config.c_str(), IN_MODIFY);
  pollfd fds[1];
  fds[0].fd = inotify_fd;
  fds[0].events = POLLIN;

  while (true) {
    /*
     * get the current time
     */
    auto now_time = system_clock::now();
    logger.log(LOG_INFO, format("{:%F %T}", now_time));

    /*
     * Gather up all the raw data
     */
    auto x_sht4x_temp = sht4x.getTemperatureMeasurement();

    auto x_sht4x_humidity = sht4x.getRelativeHumidityMeasurement();

    auto x_lps22_temp = lps22.getTemperatureMeasurement();

    auto x_lps22_pressure = lps22.getPressureMeasurement();

    /*
     * Put the raw data into the wu data
     */
    wu->setVarData("action", "updateraw");
    //time_point<utc_clock> utc_time = utc_clock::now();
    wu->setVarData("dateutc", "now");
    /*
     * Weather Underground wants fahrenheit
     */
    if (x_sht4x_temp.has_value()) {
      /*
       * The SHT4x is supposed to be more accurate so use it
       */
      qw_units::Fahrenheit tempf = x_sht4x_temp.value().fahrenheitValue();
      wu->setVarData("tempf", tempf.value());
      qw_units::Fahrenheit temp2f = x_lps22_temp.value().fahrenheitValue();
      wu->setVarData("temp2f", temp2f.value());
    }

    if (x_sht4x_humidity.has_value()) {
      qw_units::RelativeHumidity humidity =
          x_sht4x_humidity.value().relativeHumidityValue();
      wu->setVarData("humidity", humidity.value());
    }

    /*
     * If there are valid temperature and relative humidity then add a dewpoint
     */
    if (x_sht4x_temp.has_value() && x_sht4x_humidity.has_value()) {
      qw_units::Celsius tempc = x_sht4x_temp.value().celsiusValue();
      qw_units::RelativeHumidity humidity =
          x_sht4x_humidity.value().relativeHumidityValue();
      Celsius dewptc = qw_utilities::dewPoint(tempc, humidity);
      Fahrenheit dewptf = dewptc;
      wu->setVarData("dewptf", dewptf.value());
    }

    /*
     * Weather Underground wants inches mercury
     */
    if (x_lps22_pressure.has_value()) {
      qw_units::InchesMercury pressure =
          x_lps22_pressure.value().inchesMercuryValue();
      wu->setVarData("baromin", pressure.value());
    }

    /*
     * debug to check out the string
     */
    string http_request = wu->buildHttpRequest();
    logger.log(LOG_INFO, http_request);

    auto errval = wu->sendData();
    if (errval.has_value() == false) {
      logger.log(LOG_ERR, "COMM Error");
    }

    string response = wu->getHttpResponse();

    logger.log(LOG_INFO, response);

    wu->reset();

    int poll_cnt = poll(fds, 1, reporting_loop_interval);
    /*
     * If poll_cnt is zero it means the configuration file was
     * not updated and we can just cycle through and gather
     * another set of data.
     */
    if (poll_cnt != 0) {
      delete wu;
      /*
       * If we got here it means the configuration file was
       * changed. So, we have to get a new username and
       * password and then gather more data.
       */
      Json::Value json_config;
      ws_config.getRoot(json_config);
      WeatherUnderground* wu = new WeatherUnderground(
          json_config["WeatherUnderground"]["pwu_name"].asString(),
          json_config["WeatherUnderground"]["pwu_password"].asString());
      reporting_loop_interval = min(
          max(ws_report_interval_min, json_config["ReportInterval"].asInt()),
          ws_report_interval_max);
    }
  }

}
