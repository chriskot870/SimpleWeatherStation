/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains the driver for the LPS22 HB chip.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>

#include "include/lps22.h"

Lps22::Lps22(std::string i2cbus_name, uint8_t slave_address)
    : i2cbus_name_(i2cbus_name), slave_address_(slave_address) {}

uint8_t Lps22::deviceAddress() {
  return kLps22hbI2cAddress;
}

expected<uint8_t, int> Lps22::whoami() {
  expected<uint8_t, int> who_am_i;

  who_am_i = getRegister(kLps22hbWhoAmI);

  return who_am_i;
}

int Lps22::init() {
  expected<uint8_t, int> retval;
  uint8_t control_1, control_2;
  int error;

  /*
   * Make sure this is the correct device at the expected I2C address
   */
  retval = whoami();
  if (retval.has_value() != true) {
    return retval.error();
  }
  if (retval.value() != kLps22hbWhoAmIValue) {
    return 1;
  }

  /*
   * Now send a software reset
   */
  retval = getRegister(kLps22hbCtrlReg2);
  if (retval.has_value() != true) {
    return retval.error();
  }
  control_2 = retval.value();
  control_2 |= kLps22hbCtrlReg2SwResetMask;
  error = setRegister(kLps22hbCtrlReg2, control_2);
  if ( error != 0) {
    return error;
  }

  /*
     * Wait for Reset to clear
     */
  int cnt = 0;
  do {
    cnt++;
    retval =  getRegister(kLps22hbCtrlReg2);
    if (retval.has_value() != true) {
      return retval.error();
    }
    control_2 = retval.value();
  } while ((control_2 & kLps22hbCtrlReg2SwResetMask) ==
           kLps22hbCtrlReg2SwResetMask);

  /*
     * Set to the default value
     */
  control_1 = kLps22hbCtrlReg1Default;
  retval = setRegister(kLps22hbCtrlReg1, control_1);
  if (retval.has_value() != true) {
    return retval.error();
  }

  control_2 = kLps22hbCtrlReg2Default;
  retval = setRegister(kLps22hbCtrlReg2, control_2);
  if (retval.has_value() != true) {
    return retval.error();
  }

  return 0;
}

std::chrono::milliseconds Lps22::getMeasurementInterval() {
  return measurement_interval_;
}

int Lps22::setMeasurementInterval(std::chrono::milliseconds interval) {
  /*
   * Should check for some boundary conditions here
   */
  measurement_interval_ = interval;

  return 0;
}

int Lps22::getMeasurement() {
  expected <uint8_t, int> retval;
  uint8_t ctrl_register_2, data_available, pressure_available_count,
      temp_available_count;
  uint8_t buffer[] = {0, 0, 0, 0, 0};
  int error;

  measurement_count_++;
  /*
     * Start a measurement by sending a one shot command
     */
  retval = getRegister(kLps22hbCtrlReg2);
  if (retval.has_value() != true) {
    return retval.error();
  }
  ctrl_register_2 = retval.value();;
  ctrl_register_2 |= kLps22hbCtrlReg2OneShotMask;
  error = setRegister(kLps22hbCtrlReg2, ctrl_register_2);
  if (error != 0) {
    return error;
  }

  for (temp_available_count = 0; temp_available_count < 10;
       temp_available_count++) {
    retval = getRegister(kLps22hbStatus);
    if (retval.has_value() == false) {
      return retval.error();
    }
    data_available = retval.value();
    if ((data_available & kLps22hbStatusTemperatureDataAvailableMask) ==
        kLps22hbStatusTemperatureDataAvailableMask) {
      error = getRegisters(kLps22hbTempOutL, buffer, 2);
      if (error != 0) {
        return error;
      }
      temperature_measurement_ = ((buffer[1] & 0x7F) << 8) | buffer[0];
      if ((buffer[1] & 0x80) == 0x80) {
        temperature_measurement_ *= -1;
      }
      break;
    }
    usleep(10000);
  }
  for (pressure_available_count = 0; pressure_available_count < 10;
       pressure_available_count++) {
    retval = getRegister(kLps22hbStatus);
    if (retval.has_value() != true) {
      return retval.error();
    }
    data_available = retval.value();
    if ((data_available & kLps22hbStatusPressureDataAvailableMask) ==
        kLps22hbStatusPressureDataAvailableMask) {
      error = getRegisters(kLps22hbPressureOutXl, buffer, 3);
      if (error != 0) {
        return error;
      }
      pressure_measurement_ =
          ((buffer[2] & 0x7f) << 16) | buffer[1] << 8 | buffer[0];
      if ((buffer[2] & 0x80) == 0x80) {
        pressure_measurement_ *= -1;
      }
      break;
    }
    usleep(10000);
  }

  last_read_ = std::chrono::steady_clock::now();

  return 0;
}

expected <float, int> Lps22::getTemperature(TemperatureUnit_t unit) {
  uint8_t buffer[2] = {0, 0};
  float temperature;
  int error;

  if (measurementExpired()) {
    error = getMeasurement();
    if (error != 0) {
      return unexpected(error);
    }
  }

  /*
   * Peform the conversion from the data sheet
   */
  temperature =
      static_cast<float>(temperature_measurement_) / kLps22hbTemperatureFactor;

  /*
     * Convert to requested units
     */
  switch (unit) {
    case TEMPERATURE_UNIT_FAHRENHEIT:
      temperature = TemperatureConversion::celsiusToFahrenheit(temperature);
      break;
    case TEMPERATURE_UNIT_CELSIUS:
      /* It is already in celsius so just return it*/
      break;
    case TEMPERATURE_UNIT_KELVIN:
      temperature = TemperatureConversion::celsiusToKelvin(temperature);
      break;
  }

  return temperature;
}

expected <float, int>  Lps22::getBarometricPressure(PressureUnit_t unit) {
  uint8_t buffer[3] = {0, 0, 0};
  float pressure;
  int error;

  if (measurementExpired() == true) {
    error = getMeasurement();
    if (error != 0) {
      return error;
    }
  }

  pressure =
      static_cast<float>(pressure_measurement_) / kLps22hbPressureHpaFactor;
  /*
     * Return the value in the requested units
     */
  switch (unit) {
    case PRESSURE_UNIT_hPa:
      break;
    case PRESSURE_UNIT_Pa:
      pressure = PressureConversion::hPaToPa(pressure);
      break;
    case PRESSURE_UNIT_PSI:
      pressure = PressureConversion::hPaToPsi(pressure);
      break;
    case PRESSURE_UNIT_INCHES_MERCURY:
      pressure = PressureConversion::hPaTohInchesMercury(pressure);
      break;
  }

  return pressure;
}

/*
 * Private methods
 */
expected <uint8_t, int> Lps22::getRegister(uint8_t reg) {
  int retval;
  uint8_t data = 0;

  retval = getRegisters(reg, &data, 1);

  if (retval != 0) {
    return unexpected(retval);
  }

  return data;
}

int Lps22::setRegister(uint8_t reg, uint8_t data) {

  return setRegisters(reg, &data, 1);
}

int Lps22::getRegisters(uint8_t reg, uint8_t* data, uint8_t count) {
  int retval;
  int i2c_bus;

  /*
   * The lps22hb needs to write the register value to be read and then
   * perform the read. So, we need 2 fecth_serial_com for a read.
   */
  struct i2c_msg fetch_serial_com[2];
  struct i2c_rdwr_ioctl_data xfer;

  i2c_bus = open(i2cbus_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    return 1;
  }

  /*
   * Write the register of interest
   */
  fetch_serial_com[0].addr = slave_address_;
  fetch_serial_com[0].flags = 0; /* Do a write to define the first register */
  fetch_serial_com[0].len = 1;
  fetch_serial_com[0].buf = &reg;

  /*
   * Now read that register and count following it.
   */
  fetch_serial_com[1].addr = slave_address_;
  fetch_serial_com[1].flags = I2C_M_RD; /* Do a read */
  fetch_serial_com[1].len = count;
  fetch_serial_com[1].buf = data;

  xfer.msgs = fetch_serial_com;
  xfer.nmsgs = 2;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  close(i2c_bus);

  if (retval != 2) {
    return 2;
  }

  return 0;
}

int Lps22::setRegisters(uint8_t reg, uint8_t* data, uint8_t count) {
  int retval;
  int i2c_bus;
  struct i2c_msg fetch_serial_com[1];
  struct i2c_rdwr_ioctl_data xfer;
  uint8_t xfr_data[kLps22hbMaxRegistersTransferred + 1];

  /*
   * Check that the count fits within the xfr_data buffer
   */
  if (count > kLps22hbMaxRegistersTransferred) {
    return 1;
  }

  i2c_bus = open(i2cbus_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    return 2;
  }

  /*
   * Build the transfer buffer with the register as the first byte
   */
  xfr_data[0] = reg;
  memcpy(&xfr_data[1], data, count);

  /*
   * Write the command to get a measurement
   */
  fetch_serial_com[0].addr = slave_address_;
  fetch_serial_com[0].flags = 0; /* Do a write */
  fetch_serial_com[0].len =
      count +
      1;  // Write the register and then the count of bytes in the buffer
  fetch_serial_com[0].buf = xfr_data;

  xfer.msgs = fetch_serial_com;
  xfer.nmsgs = 1;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  close(i2c_bus);

  if (retval != 1) {
    return 3;
  }

  return 0;
}

bool Lps22::measurementExpired() {
  /*
   * check if the current measurement has expired
   */
  if (measurement_count_ == 0) {
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
