/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains the SHT45 information. See data sheet
 * https://cdn-shop.adafruit.com/product-files/5665/5665_Datasheet_SHT4x.pdf
 *
 */

#ifndef SRC_LIB_DEVICES_I2C_INCLUDE_SHT4X_H_
#define SRC_LIB_DEVICES_I2C_INCLUDE_SHT4X_H_

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <expected>
#include <string>
#include <map>
#include <mutex>
#include <vector>

/*
 * This device has temperature and relative humidity sensors so add the interfaces
 * and measurements.
 */
#include "relative_humidity_interface.h"
#include "temperature_interface.h"

#include "temperature_measurement.h"
#include "relative_humidity_measurement.h"

/*
 * This is an i2c bus device so add the i2cbus.h
 */
#include "include/i2cbus.h"

using std::expected;
using std::string;
using std::unexpected;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::steady_clock;
using std::chrono::milliseconds;
using std::atomic_bool;
using std::vector;
using std::map;
using std::mutex;
using std::recursive_mutex;
using std::find;

/*
 * Fixed address. could be 0x45 you have to check the model from the data sheet
 */
constexpr uint8_t kSht4xI2cPrimaryAddress = 0x44;
constexpr uint8_t kSht4xI2cSecondaryAddress = 0x45;

const vector<uint8_t> sht4x_slave_address_options = { kSht4xI2cPrimaryAddress, kSht4xI2cSecondaryAddress };

constexpr std::chrono::milliseconds kDefaultMeasurementInterval(
    2000); /* The number of msecs that a reading is good */
constexpr std::chrono::milliseconds kMinimumMeasurementInterval(
    1000); /* The minimum value for measurement_interval_ */
constexpr std::chrono::milliseconds kSteadyClockZero(0);
/*
 * Commands
 */
constexpr uint8_t kSht4xResponseLength = 6;  // All commands have same response length
constexpr uint8_t kSht4xCommandLength = 1;
constexpr uint8_t kSht4xSerialReturnLength = 6;
constexpr uint8_t kSht4xResetLength = 1;
constexpr uint8_t kSht4xReadDataLength = 6;
constexpr uint8_t kSht4xDataTemperatureMsbOffset = 0;
constexpr uint8_t kSht4xDataTemperatureLsbOffset = 1;
constexpr uint8_t kSht4xDataTemperatureCrcOffset = 2;
constexpr uint8_t kSht4xDataRelativeHumidityMsbOffset = 3;
constexpr uint8_t kSht4xDataRelativeHumidityLsbOffset = 4;
constexpr uint8_t kSht4xDataRelaiveHumidityCrcOffset = 5;

constexpr uint8_t kSht4xCommandHighPrecisionMeasurement = 0xFD;
constexpr uint8_t kSht4xCommandMediumPrecisionMeasurement = 0xF6;
constexpr uint8_t kSht4xCommandLowPrecisionMeasurement = 0xE0;
constexpr uint8_t kSht4xCommandReadSerialNumber = 0x89;
constexpr uint8_t kSht4xCommandReset = 0x94;
constexpr uint8_t kSht4xCommandActivateHtr200mwOneSecond = 0x39;
constexpr uint8_t kSht4xCommandActivateHtr200mwTenthSecond = 0x32;
constexpr uint8_t kSht4xCommandActivateHtr110mwOneSecond = 0x2F;
constexpr uint8_t kSht4xCommandActivateHtr110mwTenthSecond = 0x24;
constexpr uint8_t kSht4xCommandActivateHtr20mwOneSecond = 0x1E;
constexpr uint8_t kSht4xCommandActivateHtr20mwTenthSecond = 0x15;

constexpr int kSht4xTemperatureCelsiusMultiplier = 175;
constexpr int kSht4xTemperatureCelsisusDivisor = 65535;
constexpr int kSht4xTemperatureCelsiusOffset = 45;

constexpr int kSht4xTemperatureFarenheitMultiplier = 315;
constexpr int kSht4xTemperatureFarenheitDivisor = 65535;
constexpr int kSht4xTemperatureFahrenheitOffset = 49;

constexpr int kSht4xRelativeHumidityMultiplier = 125;
constexpr int kSht4xRelativeHumidityDivisor = 65535;
constexpr int kSht4xRelativeHumidityOffset = 6;

constexpr int kSht4xTemperatureKelvinMultiplier = kSht4xTemperatureCelsiusMultiplier;
constexpr int kSht4xTemperatureKelvinDivisor = kSht4xTemperatureCelsisusDivisor;
constexpr float kSht4xTemperatureKelvinOffset =
    kSht4xTemperatureCelsiusOffset + 273.15;

typedef enum {
  SHT4X_MEASUREMENT_PRECISION_HIGH,
  SHT4X_MEASUREMENT_PRECISION_MEDIUM,
  SHT4X_MEASUREMENT_PRECISION_LOW,
  SHT4X_HEATER_ACTkSIVATION_200mW_ONE_SECOND,
  SHT4X_HEATER_ACTIVATION_200mW_TENTH_SECOND,
  SHT4X_HEATER_ACTIVATION_110mW_ONE_SECOND,
  SHT4X_HEATER_ACTIVATION_110mW_TENTH_SECOND,
  SHT4X_HEATER_ACTIVATION_20mW_ONE_SECOND,
  SHT4X_HEATER_ACTIVATION_20mW_TENTH_SECOND,
} Sht4xMeasurmentMode;

/*
 * The order here has to match the enum above so that you get the right value inthe array
 */
const uint8_t sht3x_measurement_command_map[] = {
    kSht4xCommandHighPrecisionMeasurement,
    kSht4xCommandMediumPrecisionMeasurement,
    kSht4xCommandLowPrecisionMeasurement,
    kSht4xCommandActivateHtr200mwOneSecond,
    kSht4xCommandActivateHtr200mwTenthSecond,
    kSht4xCommandActivateHtr110mwOneSecond,
    kSht4xCommandActivateHtr110mwTenthSecond,
    kSht4xCommandActivateHtr20mwOneSecond,
    kSht4xCommandActivateHtr20mwTenthSecond};

/*
 * These are delay times from when a command is given to when it completes.
 * See section 3.1 of the Datasheet.
 */
typedef enum {
  SHT4X_TIMING_SOFT_RESET,
  SHT4X_TIMING_MEASUREMENT_LOW_REPEATABILITY,
  SHT4X_TIMING_MEASUREMENT_MED_REPEATABILITY,
  SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY,
  SHT4X_TIMING_HEATER_DURATION_LONG,
  SHT4X_TIMING_HEATER_DURATION_SHORT,
} Sht4xMaxTimings;

/*
 * These are in microseonds
 */
const int shtx_max_timings[] = {
    1000, /* tpu 1 millisecond */
    /*
     * For the next 3 the table says including tpu. I take that
     * to mean I should add the value above, tpu, to these values
     * in the table. I may have that wrong though.
     */
    1600 + 1000, /* 1.6 millisecond  + tpu = 2.6 milliseconds*/
    4500 + 1000, /* 4.5 milliseconds + tpu = 5.5 milliseconds */
    8300 + 1000, /* 8.3 milliseconds + tpu = 9.3 milliseconds */
    1100000,     /* 1.1 seconds */
    110000       /* 0.11 seconds */
};

class Sht4xDeviceLocation {
 public:
  string bus_name_;
  uint8_t slave_address_;

  /*
   * We need the == comparison to support the contain function for this class
   * to be used as a key in a map.
   */
  bool operator==(const Sht4xDeviceLocation& data) const {
    if ((bus_name_ == data.bus_name_) && (slave_address_ == data.slave_address_)) {
      return true;
    }
    return false;
  }

  /*
   * If you have == you should also have !=
   */
  bool operator!=(const Sht4xDeviceLocation& data) const {
    if ((bus_name_ == data.bus_name_) && (slave_address_ == data.slave_address_)) {
      return false;
    }
    return true;
  }

  /*
   * We need the < operator in order to use this class as a key for a map
   * We set the order that the busname is checked then slave on that bus
   */
  bool operator<(const Sht4xDeviceLocation& data) const {
    if (bus_name_.compare(data.bus_name_) < 0) return true;
    if (bus_name_.compare(data.bus_name_) > 0) return false;
    if (slave_address_ < data.slave_address_) return true;
    return false;
  }
};

class Sht4xDeviceData {
 public:

  recursive_mutex lock_ = {};
  uint64_t read_total_ = 0;
  atomic_bool initialized = false;
};

class I2cSht4x : public TemperatureInterface, public RelativeHumidityInterface {
 public:
  I2cSht4x(I2cBus i2cbus_, uint8_t slave_address);

  uint8_t deviceAddress();

  int init();

  expected<uint32_t, int> getSerialNumber();

  int softReset();

  expected<TemperatureMeasurement, int> getTemperatureMeasurement(TemperatureUnit_t unit);

  expected<RelativeHumidityMeasurement, int> getRelativeHumidityMeasurement();

  std::chrono::milliseconds getMeasurementInterval();

  int setMeasurementInterval(milliseconds interval);

  int error_code();

  string error_message();

 private:
   /*
    * Private Data
    */
  Sht4xDeviceLocation device_;
  Sht4xDeviceData* device_data_ = nullptr;
  // Number of measurements made
  uint64_t measure_count_ = 0;

  // The slave address of the device. The sht45 can be either 0x44 or 0x45.
  uint8_t slave_address_;

  // Which I2c bus is the device on
  I2cBus i2cbus_;

  // minimum interval between making a measurement.
  std::chrono::milliseconds measurement_interval_ =
      kDefaultMeasurementInterval; /* Interval between measurements */

  // The last time point a measurement was made. Initialize to zero.
  time_point<steady_clock> last_read_{};

  // Temperature measurement
  uint16_t temperature_measurement_;

  time_point<system_clock>  temperature_measurement_time_;

  time_point<system_clock> temperature_measurement_clock_time_;

  // Humidity measurement
  uint16_t relative_humidity_measurement_;
  time_point<system_clock>  relative_humidity_measurement_time_;

  time_point<system_clock> relativehumidity_measurement_clock_time_;

  // Serial Number
  uint32_t serial_number_ = 0;

  int error_code_ = 0;

  string error_message_ = {};
  /*
   * Private Functions
   */
  int getMeasurement(Sht4xMeasurmentMode mode);

  bool measurementExpired();
};

#endif  // SRC_LIB_DEVICES_I2C_INCLUDE_SHT4X_H_
