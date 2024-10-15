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

Lps22::Lps22(I2cBus i2cbus, uint8_t slave_address)
    : i2cbus_(i2cbus), slave_address_(slave_address) {}

uint8_t Lps22::deviceAddress() {
  return kLps22hbI2cAddress;
}

expected<uint8_t, int> Lps22::whoami() {
  int retval;
  uint8_t who_am_i;

  retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbWhoAmI,
                                             &who_am_i, sizeof(who_am_i));

  if (retval != 0) {
    return unexpected(retval);
  }

  return who_am_i;
}

int Lps22::init() {
  int retval;
  expected<uint8_t, int> x_return;
  uint8_t control_1, control_2;
  int error;

  /*
   * Make sure this is the correct device at the expected I2C address
   */
  x_return = whoami();
  if (x_return.has_value() == false) {
    /*
     * Return whatever error was found at the lower area
     */
    return x_return.error();
  }
  if (x_return.value() != kLps22hbWhoAmIValue) {
    /*
     * If the value is not a match record it as a communication error
     */
    return ECOMM;
  }

  /*
   * Now send a software reset
   */
  retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbCtrlReg2,
                                             &control_2, sizeof(control_2));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
  }
  control_2 |= kLps22hbCtrlReg2SwResetMask;
  retval = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg2,
                                           &control_2, sizeof(control_2));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
  }

  /*
     * Wait for Reset to clear
     */
  int cnt = 0;
  do {
    cnt++;
    retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbCtrlReg2,
                                               &control_2, sizeof(control_2));
    if (retval != 0) {
      /*
       * Return whatever error was found at the lower area
       */
      return retval;
    }
  } while (((control_2 & kLps22hbCtrlReg2SwResetMask) ==
            kLps22hbCtrlReg2SwResetMask) &&
           cnt < kLps22ResetWaitCount);

  /*
     * Set to the default value
     */
  control_1 = kLps22hbCtrlReg1Default;
  retval = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg1,
                                           &control_1, sizeof(control_1));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
  }

  control_2 = kLps22hbCtrlReg2Default;
  retval = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg2,
                                           &control_2, sizeof(control_2));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
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
  int retval;
  uint8_t ctrl_register_2, data_available, pressure_available_count,
      temp_available_count;
  uint8_t temp_buffer[] = {0, 0};
  uint8_t pres_buffer[] = {0, 0};
  int error;

  measurement_count_++;
  /*
     * Start a measurement by sending a one shot command
     */
  retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbCtrlReg2,
                                             &ctrl_register_2,
                                             sizeof(ctrl_register_2));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
  }
  ctrl_register_2 |= kLps22hbCtrlReg2OneShotMask;
  error = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg2,
                                          &ctrl_register_2,
                                          sizeof(ctrl_register_2));
  if (error != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return error;
  }

  for (temp_available_count = 0; temp_available_count < 10;
       temp_available_count++) {
    retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbStatus,
                                             &data_available,
                                             sizeof(data_available));
    if (retval != 0) {
      /*
       * Return whatever error was found at the lower area
       */
      return retval;
    }
    if ((data_available & kLps22hbStatusTemperatureDataAvailableMask) ==
        kLps22hbStatusTemperatureDataAvailableMask) {
      error = i2cbus_.transferDataFromRegisters(
          slave_address_, kLps22hbTempOutL, temp_buffer, sizeof(temp_buffer));
      if (error != 0) {
        /*
         * Return whatever error was found at the lower area
         */
        return error;
      }
      temperature_measurement_ =
          ((temp_buffer[1] & 0x7F) << 8) | temp_buffer[0];
      if ((temp_buffer[1] & 0x80) == 0x80) {
        temperature_measurement_ *= -1;
      }
      break;
    }
    usleep(10000);
  }
  for (pressure_available_count = 0; pressure_available_count < 100;
       pressure_available_count++) {

    retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbStatus,
                                               &data_available,
                                               sizeof(data_available));
    if (retval != 0) {
      /*
       * Return whatever error was found at the lower area
       */
      return retval;
    }
    if ((data_available & kLps22hbStatusPressureDataAvailableMask) ==
        kLps22hbStatusPressureDataAvailableMask) {
      error = i2cbus_.transferDataFromRegisters(
          slave_address_, kLps22hbPressureOutXl, pres_buffer,
          sizeof(pres_buffer));
      if (error != 0) {
        /*
         * Return whatever error was found at the lower area
         */
        return error;
      }
      pressure_measurement_ = ((pres_buffer[2] & 0x7f) << 16) |
                              pres_buffer[1] << 8 | pres_buffer[0];
      if ((pres_buffer[2] & 0x80) == 0x80) {
        pressure_measurement_ *= -1;
      }
      break;
    }
    usleep(10000);
  }

  last_read_ = std::chrono::steady_clock::now();

  return 0;
}

expected<float, int> Lps22::getTemperature(TemperatureUnit_t unit) {
  uint8_t buffer[2] = {0, 0};
  float temperature;
  int error;

  if (measurementExpired()) {
    error = getMeasurement();
    if (error != 0) {
      /*
       * Return whatever error was found at the lower area
       */
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

expected<float, int> Lps22::getBarometricPressure(PressureUnit_t unit) {
  uint8_t buffer[3] = {0, 0, 0};
  float pressure;
  int error;

  if (measurementExpired() == true) {
    error = getMeasurement();
    if (error != 0) {
      /*
       * Return whatever error was found at the lower area
       */
      error_code_ = error;
      return unexpected(error_code_);
    }
  }

  pressure =
      static_cast<float>(pressure_measurement_) / kLps22hbPressureHpaFactor;
  /*
   * Return the value in the requested units
   */
  switch (unit) {
    case PRESSURE_UNIT_Mb:
      break;
    case PRESSURE_UNIT_Pa:
      pressure = PressureConversion::MbToPa(pressure);
      break;
    case PRESSURE_UNIT_PSI:
      pressure = PressureConversion::MbToPsi(pressure);
      break;
    case PRESSURE_UNIT_INCHES_MERCURY:
      pressure = PressureConversion::MbTohInchesMercury(pressure);
      break;
  }

  return pressure;
}

/*
 * Private methods
 */
expected<uint8_t, int> Lps22::getRegister(uint8_t reg) {
  int retval;
  uint8_t data = 0;

  retval = i2cbus_.transferDataFromRegisters(slave_address_, reg, &data,
                                             sizeof(data));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return unexpected(retval);
  }

  return data;
}

int Lps22::setRegister(uint8_t reg, uint8_t data) {
  int retval;

  retval =
      i2cbus_.transferDataToRegisters(slave_address_, reg, &data, sizeof(data));

  return retval;
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
