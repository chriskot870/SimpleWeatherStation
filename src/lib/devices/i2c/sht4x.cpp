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

#include "include/sht4x.h"

I2cSht4x::I2cSht4x(std::string i2cbus_name, uint8_t slave_address)
    : i2cbus_name_(i2cbus_name), slave_address_(slave_address) {}

uint8_t I2cSht4x::deviceAddress() {
  return slave_address_;
}

bool I2cSht4x::getSerialNumber(uint32_t* serial_number) {
  int32_t retval;
  int i2c_bus;
  uint8_t command = kSht4xCommandReadSerialNumber;
  uint8_t read_buffer[kSht4xSerialReturnLength] = {0, 0, 0, 0, 0, 0};
  struct i2c_msg fetch_serial_com;
  struct i2c_rdwr_ioctl_data xfer;

  /*
    * The serial number shouldn't change so we only have to get it once.
    * Check to see if all elements of the private serial number are 0.
    * If so then it means we haven't read the serial number before.
    * So, read it now.
    */

  if (serial_number_ != 0) {
    *serial_number = serial_number_;
    return true;
  }

  /*
     * If I get here then I need to fetch the serial number
     */
  i2c_bus = open(i2cbus_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    return false;
  }

  /*
     * Write the serial number command to the address
     */
  fetch_serial_com.addr = slave_address_;
  fetch_serial_com.flags = 0;
  fetch_serial_com.len = kSht4xCommandLength;
  fetch_serial_com.buf = &command;

  xfer.msgs = &fetch_serial_com;
  xfer.nmsgs = 1;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  if (retval < 0) {
    close(i2c_bus);
    return false;
  }

  /*
     * I seem to need a delay here so use the high reliability time
     */
  usleep(shtx_max_timings[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);
  /*
     * Perform the read to get the serial number
     */
  fetch_serial_com.addr = slave_address_;
  fetch_serial_com.flags = I2C_M_RD;
  fetch_serial_com.len = kSht4xSerialReturnLength;
  fetch_serial_com.buf = read_buffer;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  if (retval < 0) {
    close(i2c_bus);
    return false;
  }
  /*
     * I am not sure I need this extra or not.
     * This could be just tpu value but for now give me plenty of delay
     */
  usleep(shtx_max_timings[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
     * We need figure out how to check the CRC's
     */
  serial_number_ = (read_buffer[0] << 24) + (read_buffer[1] << 16) +
                   (read_buffer[3] << 8) + read_buffer[4];

  close(i2c_bus);

  *serial_number = serial_number_;

  return true;
}

bool I2cSht4x::softReset() {
  int32_t retval;
  int i2c_bus;
  uint8_t command = kSht4xCommandReset;
  struct i2c_msg fetch_serial_com;
  struct i2c_rdwr_ioctl_data xfer;

  /*
     * If I get here then I need to fetch the serial number
     */
  i2c_bus = open(i2cbus_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    return false;
  }

  fetch_serial_com.addr = slave_address_;
  fetch_serial_com.flags = 0;
  fetch_serial_com.len = kSht4xCommandLength;
  fetch_serial_com.buf = &command;

  xfer.msgs = &fetch_serial_com;
  xfer.nmsgs = 1;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  if (retval != 1) {
    close(i2c_bus);
    return false;
  }
  /*
     * Give it time to do the reset
     */
  usleep(shtx_max_timings[SHT4X_TIMING_SOFT_RESET]);

  close(i2c_bus);

  return true;
}

float I2cSht4x::getTemperature(TemperatureUnit_t unit) {
  float temperature;
  if (measurementExpired() == true) {
    getMeasurement(SHT4X_MEASUREMENT_PRECISION_HIGH);
  }

  switch (unit) {
    case TEMPERATURE_UNIT_FAHRENHEIT:
      temperature = (kSht4xTemperatureFahrenheitSlope *
                     static_cast<float>(temperature_measurement_)) -
                    kSht4xTemperatureFahrenheitOffset;
      break;
    case TEMPERATURE_UNIT_CELSIUS:
      temperature = (kSht4xTemperatureCelsiusSlope *
                     static_cast<float>(temperature_measurement_)) -
                    kSht4xTemperatureCelsiusOffset;
      break;
    case TEMPERATURE_UNIT_KELVIN:
      temperature = (kSht4xTemperatureKelvinSlope *
                     static_cast<float>(temperature_measurement_)) -
                    kSht4xTemperatureKelvinOffset;
      break;
  }

  return temperature;
}

std::chrono::milliseconds I2cSht4x::getMeasurementInterval() {
  return measurement_interval_;
}

void I2cSht4x::setMeasurementInterval(std::chrono::milliseconds interval) {
  /*
   * If the request is for less than the minimum interval allowed make it the minimum
   */
  if (interval < kMinimumMeasurementInterval) {
    measurement_interval_ = kMinimumMeasurementInterval;
  } else {
    measurement_interval_ = interval;
  }

  return;
}

float I2cSht4x::getRelativeHumidity() {
  float relative_humidity;

  if (measurementExpired() == true) {
    getMeasurement(SHT4X_MEASUREMENT_PRECISION_HIGH);
  }

  relative_humidity = (kSht4xRelativeHumiditySlope *
                       static_cast<float>(relative_humidity_measurement_)) -
                      kSht4xRelativeHumidityOffset;

  /*
     * SHT4x document says:
     * cropping of the RH signal to the range of 0 %RH … 100 %RH is advised.
     */
  if (relative_humidity < 0.0) {
    return 0.0;
  }
  if (relative_humidity > 100.0) {
    return 100.0;
  }

  return relative_humidity;
}

/*
 * Private Methods
 */
void I2cSht4x::getMeasurement(Sht4xMeasurmentMode mode) {
  int retval;
  int i2c_bus;
  struct i2c_msg fetch_serial_com;
  struct i2c_rdwr_ioctl_data xfer;
  uint8_t command = sht3x_measurement_command_map[mode];
  uint8_t read_buffer[] = {0, 0, 0, 0, 0, 0};

  /* Increment the counter that monitors how often this routine gets called*/
  measure_count_++;

  i2c_bus = open(i2cbus_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    return;
  }

  /*
     * Write the command to get a measurement
     */
  fetch_serial_com.addr = slave_address_;
  fetch_serial_com.flags = 0; /* Do a write */
  fetch_serial_com.len = kSht4xCommandLength;
  fetch_serial_com.buf = &command;

  xfer.msgs = &fetch_serial_com;
  xfer.nmsgs = 1;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  if (retval < 0) {
    return;
  }
  /*
     * Give it time to make the measurement
     */
  usleep(shtx_max_timings[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);
  /*
     * Perform the read to get the measurement
     */
  fetch_serial_com.addr = slave_address_;
  fetch_serial_com.flags = I2C_M_RD;
  fetch_serial_com.len = kSht4xSerialReturnLength;
  fetch_serial_com.buf = read_buffer;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  if (retval < 0) {
    return;
  }
  close(i2c_bus);

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

  return;
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
