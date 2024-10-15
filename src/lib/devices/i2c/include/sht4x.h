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

#include "relative_humidity_interface.h"
#include "temperature_interface.h"

/*
 * This is an i2c bus device so add the i2cbus.h
 */
#include "include/i2cbus.h"

using std::expected;
using std::string;
using std::unexpected;

/*
 * Fixed address. could be 0x45 you have to check the model from the data sheet
 */
constexpr uint8_t kSht4xI2cPrimaryAddress = 0x44;
constexpr uint8_t kSht4xI2cSecondaryAddress = 0x45;

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

constexpr float kSht4xTemperatureCelsiusSlope =
    (static_cast<float>(175) / static_cast<float>(65535));
constexpr int kSht4xTemperatureCelsiusOffset = 45;
constexpr float kSht4xTemperatureFahrenheitSlope =
    (static_cast<float>(315) / static_cast<float>(65535));
constexpr int kSht4xTemperatureFahrenheitOffset = 49;
constexpr float kSht4xRelativeHumiditySlope =
    (static_cast<float>(125) / static_cast<float>(65535));
constexpr int kSht4xRelativeHumidityOffset = 6;
constexpr float kSht4xTemperatureKelvinSlope = kSht4xTemperatureCelsiusSlope;
constexpr float kSht4xTemperatureKelvinOffset =
    kSht4xTemperatureCelsiusOffset + 273.15;

typedef enum {
  SHT4X_MEASUREMENT_PRECISION_HIGH,
  SHT4X_MEASUREMENT_PRECISION_MEDIUM,
  SHT4X_MEASUREMENT_PRECISION_LOW,
  SHT4X_HEATER_ACTIVATION_200mW_ONE_SECOND,
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

class I2cSht4x : public TemperatureInterface, public RelativeHumidityInterface {
 public:
  I2cSht4x(I2cBus i2cbus_, uint8_t slave_address);

  uint8_t deviceAddress();

  int init();

  expected<uint32_t, int> getSerialNumber();

  int softReset();

  expected<float, int> getTemperature(TemperatureUnit_t unit);

  expected<float, int> getRelativeHumidity();

  std::chrono::milliseconds getMeasurementInterval();

  int setMeasurementInterval(std::chrono::milliseconds interval);

  int error_code();

  string error_message();

 private:
  /*
     * Private Data
     */

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
  std::chrono::time_point<std::chrono::steady_clock> last_read_{};

  // Temperature measurement
  uint16_t temperature_measurement_;

  // Humidity measurement
  uint16_t relative_humidity_measurement_;

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
