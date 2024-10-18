/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains the driver for the SHT4x series. Specifically the SHT45
 */

#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <cmath>

#include "include/i2cbus.h"
#include "include/sht4x.h"

I2cSht4x::I2cSht4x(I2cBus i2cbus, uint8_t slave_address)
    : i2cbus_(i2cbus), slave_address_(slave_address) {}

uint8_t I2cSht4x::deviceAddress() {
  return slave_address_;
}

expected<uint32_t, int> I2cSht4x::getSerialNumber() {
  int32_t retval;
  int i2c_bus;
  uint8_t command = kSht4xCommandReadSerialNumber;

  /*
   * Serial is two 16 bit values with a CRC after each one. So we need to read 6 bytes
   */
  uint8_t read_buffer[kSht4xSerialReturnLength] = {0, 0, 0, 0, 0, 0};

  /*
   * The serial number shouldn't change so we only have to get it once.
   * Check to see if all elements of the private serial number are 0.
   * If so then it means we haven't read the serial number before.
   * So, read it now.
   */

  if (serial_number_ != 0) {
    return serial_number_;
  }

  /*
   * Write the serial number command to the device
   */
  retval = i2cbus_.writeCommand(slave_address_, &command, sizeof(command));

  /*
   * I seem to need a delay here so use the high reliability time
   */
  usleep(shtx_max_timings[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
   * Now get the result of the command
   */
  retval = i2cbus_.readCommandResult(slave_address_, read_buffer,
                                     sizeof(read_buffer));
  /*
   * I am not sure I need this extra or not.
   * This could be just tpu value but for now give me plenty of delay
   */
  usleep(shtx_max_timings[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
   * read_buffer high order bytes are in offsets 0 and 1. Offset 2 is the
   * CRC for offset 0 and 1. Offsets 3 and 4 are the lower order bytes
   * followed by a CRC of offset 3 and 4 in offset 5.
   */
  serial_number_ = (read_buffer[0] << 24) + (read_buffer[1] << 16) +
                   (read_buffer[3] << 8) + read_buffer[4];

  return serial_number_;
}

int I2cSht4x::softReset() {
  int32_t retval;
  uint8_t command = kSht4xCommandReset;

  /*
   * Write the command to the devices
   */
  retval = i2cbus_.writeCommand(slave_address_, &command, sizeof(command));
  /*
   * Give it time to do the reset
   */
  usleep(shtx_max_timings[SHT4X_TIMING_SOFT_RESET]);

  return 0;
}

expected<TemperatureMeasurement, int> I2cSht4x::getTemperatureMeasurement(TemperatureUnit_t unit) {
  int error;
  float temperature;

  if (measurementExpired() == true) {
    error = getMeasurement(SHT4X_MEASUREMENT_PRECISION_HIGH);
    if (error != 0) {
      return unexpected(error);
    }
  }

  switch (unit) {
    case TEMPERATURE_UNIT_FAHRENHEIT:
      temperature = ((kSht4xTemperatureFarenheitMultiplier * static_cast<float>(temperature_measurement_))/kSht4xTemperatureFarenheitDivisor) -
                    kSht4xTemperatureFahrenheitOffset;
      break;
    case TEMPERATURE_UNIT_CELSIUS:
      temperature = ((kSht4xTemperatureCelsiusMultiplier * static_cast<float>(temperature_measurement_))/kSht4xTemperatureCelsisusDivisor) -
                    kSht4xTemperatureCelsiusOffset;
      break;
    case TEMPERATURE_UNIT_KELVIN:
      temperature = ((kSht4xTemperatureKelvinMultiplier * static_cast<float>(temperature_measurement_))/kSht4xTemperatureKelvinDivisor) -
                    kSht4xTemperatureKelvinOffset;
      break;
  }

  /*
   * DEBUG: How to get degrees milli-celsius. We may want to store the base
   * temperature in and then the others are derived from it.
   *
  int milli_c = round((((kSht4xTemperatureCelsiusMultiplier * static_cast<float>(temperature_measurement_))/kSht4xTemperatureCelsisusDivisor) -
                    kSht4xTemperatureCelsiusOffset) * 1000);
   */

  temperature_measurement_clock_time_ = std::chrono::system_clock::now();
  
  TemperatureMeasurement measurement(TemperatureDatum(temperature, unit), temperature_measurement_clock_time_);

  return measurement;
}

std::chrono::milliseconds I2cSht4x::getMeasurementInterval() {
  return measurement_interval_;
}

int I2cSht4x::setMeasurementInterval(std::chrono::milliseconds interval) {
  /*
   * If the request is for less than the minimum interval allowed make it the minimum
   * May want to return an error instead
   */
  if (interval < kMinimumMeasurementInterval) {
    measurement_interval_ = kMinimumMeasurementInterval;
  } else {
    measurement_interval_ = interval;
  }

  return 0;
}

expected<RelativeHumidityMeasurement, int> I2cSht4x::getRelativeHumidityMeasurement() {
  float relative_humidity;
  int error;

  if (measurementExpired() == true) {
    error = getMeasurement(SHT4X_MEASUREMENT_PRECISION_HIGH);
    if (error != 0) {
      return unexpected(error);
    }
  }

  relative_humidity = ((kSht4xRelativeHumidityMultiplier * static_cast<float>(relative_humidity_measurement_))/kSht4xRelativeHumidityDivisor) -
                      kSht4xRelativeHumidityOffset;

  /*
   * SHT4x document says:
   * cropping of the RH signal to the range of 0 %RH … 100 %RH is advised.
   */
  if (relative_humidity < 0.0) {
    relative_humidity = 0.0;
  }
  if (relative_humidity > 100.0) {
    relative_humidity = 100.0;
  }

  relativehumidity_measurement_clock_time_ = std::chrono::system_clock::now();

  RelativeHumidityDatum rhdata(relative_humidity, RELATIVE_HUMIDITY_UNIT_PERCENT);
  
  RelativeHumidityMeasurement measurement(rhdata , relativehumidity_measurement_clock_time_);

  return measurement;
}

/*
 * Private Methods
 */
int I2cSht4x::getMeasurement(Sht4xMeasurmentMode mode) {
  int retval;
  int i2c_bus;
  struct i2c_msg fetch_serial_com;
  struct i2c_rdwr_ioctl_data xfer;
  uint8_t command = sht3x_measurement_command_map[mode];
  /*
   * The data is transferred in the following format
   * Offsets: 0,1 16 bits of temperature
   *          2   8 bits of CRC for temperature
   *          3,4 16 bits of humidity
   *          5   8 bits of CRC for humidity
   */

  uint8_t read_buffer[kSht4xResponseLength] = {0, 0, 0, 0, 0, 0};

  /*
   * Increment the counter that monitors how often this routine gets called
   */
  measure_count_++;

  i2cbus_.writeCommand(slave_address_, &command, sizeof(command));

  /*
   * Give it time to make the measurement
   */
  usleep(shtx_max_timings[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
   * Now read the result
   */
  i2cbus_.readCommandResult(slave_address_, read_buffer, sizeof(read_buffer));

  /*
   * I am not sure I need this extra or not.
   * This could be just tpu value but for now give me plenty of delay
   */
  usleep(shtx_max_timings[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
   * We need figure out how to check the CRC's
   */
  temperature_measurement_ = (read_buffer[0] << 8) + read_buffer[1];
  relative_humidity_measurement_ = (read_buffer[3] << 8) + read_buffer[4];

  last_read_ = std::chrono::steady_clock::now();

  return 0;
}

bool I2cSht4x::measurementExpired() {
  /*
   * check if the current measurement has expired
   */
  if (measure_count_ == 0) {
    return true;
  }
  std::chrono::time_point<std::chrono::steady_clock> now =
      std::chrono::steady_clock::now();

  auto time_diff =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - last_read_);

  if (time_diff > measurement_interval_) {
    return true;
  }

  return false;
}
