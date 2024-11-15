/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains the driver for the LPS22 HB chip.
 */
#include "include/lps22.h"

mutex lps22_devices_lock;
//static map<Lps22DeviceLocation, Lps22DeviceData*> lps22_devices;
static map<Lps22DeviceLocation, shared_ptr<Lps22DeviceData>> lps22_devices;

/*
 * Constructor
 */
Lps22::Lps22(I2cBus i2cbus, uint8_t slave_address)
    : i2cbus_(i2cbus), slave_address_(slave_address) {
  
   /*
   * Check that the i2c bus name is a valid name
   */
  if (i2cbus.busName().compare(0, i2c_devicename_prefix.size(),i2c_devicename_prefix) != 0) {
    return;
  }
  /*
   * Check that slave addresses are valid
   */
  auto item = find(lps22_slave_address_options.begin(), lps22_slave_address_options.end(), slave_address_);
  if (item == lps22_slave_address_options.end()) {
    return;
  }

  /*
   * If the names are valid, Create the device
   */
  device_.bus_name_ = i2cbus.busName();
  device_.slave_address_ = slave_address;

  /*
   * If the device is already on the list then some other instance has
   * validated it and we don't need to add it to the list
   */
  lock_guard<mutex> guard_devices(lps22_devices_lock);
  if (lps22_devices.contains(device_) == true) {
    /*
     * Some previous instance has added the device to the devices list
     */
    device_data_ = lps22_devices[device_];
    return;
  }

  /*
   * Create a DeviceData and use it to see if whoami works
   * If whoami does not work then reset device_data_
   * 
   * TODO: I should be using make_shared but it doesn't compile.
   */
  device_data_ = shared_ptr<Lps22DeviceData>(new Lps22DeviceData());

  expected<uint8_t, int> x_return;
  x_return = whoami();
  if (x_return.has_value() == false) {
    /*
     * I can't get the whoami so reset device_data_ to nullptr
     * to indicate the device doesn't seem to be at the bus and slave
     * address provided.
     * This causes all other routines to call an ENODEV error
     */
    device_data_.reset();
    return;
  }

  /*
   * If I get here I got a value. Make sure it matches the 
   */
  if (x_return.value() != kLps22hbWhoAmIValue) {
    /*
     * free up the memory and set the device_data_ to nullptr
     */
    device_data_.reset();
    return;
  }
  lps22_devices[device_] = device_data_;

  return;
}

/*
 * Return the whoami value on the device
 */
expected<uint8_t, int> Lps22::whoami() {
  int retval;
  uint8_t who_am_i;

  if (device_data_ == nullptr) {
    return unexpected(retval);
  }

  lock_guard<recursive_mutex> guard(device_data_->lock_);  /* get the device lock
                                                     * device lock will be unlcoked when
                                                     * guard's destruct routine gets called
                                                     */

  retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbWhoAmI,
                                             &who_am_i, sizeof(who_am_i));

  if (retval != 0) {
    return unexpected(retval);
  }

  return who_am_i;
}

int Lps22::reset() {
  int retval;
  expected<uint8_t, int> x_return;
  uint8_t control_1, control_2;
  int error;

  if (device_data_ == nullptr) {
    return ENODEV;
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);  /* get the device lock
                                                     * device lock will be unlcoked when
                                                     * guard's destruct routine gets called
                                                     */
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

milliseconds Lps22::getMeasurementInterval() {
  /*
   * Return the minimum interval allowed
   */
  return measurement_interval_;
}

int Lps22::init() {
  expected<uint8_t, int> x_return;
  int retval = 0;

  if (device_data_ == nullptr) {
    return ENODEV;
  }
  /*
   * Make sure this is the correct device at the expected I2C address
   * It is assumed we can always get the whoami value.
   */
  x_return = whoami();
  /*
   * If I can't do a whoami we probably have the wrong device so
   * clear device_data_
   */
  if (x_return.has_value() == false) {
    /*
     * Return whatever error was found at the lower area reort it.
     * We assume the deice isn't at the bus,slave location so
     * free the device_ptr_
     */
    return x_return.error();
  }
  if (x_return.value() != kLps22hbWhoAmIValue) {
    /*
     * If the value is not a match record it as a Bad Message
     */
    return EBADMSG;
  }

  retval = reset();

  return retval;
}


int Lps22::setMeasurementInterval(milliseconds interval) {
  /*
   * TODO:
   * Should check for some boundary conditions here
   */
  measurement_interval_ = interval;

  return 0;
}

int Lps22::getMeasurement() {
  int retval;
  uint8_t ctrl_register_2, data_available, pressure_available_count,
      temp_available_count;
  uint8_t temp_buffer[] = {0, 0};  // Temperature is 2 bytes
  uint8_t pres_buffer[] = {0, 0, 0};  // Pressure is 3 bytes
  bool temp_updated = false, pres_updated = false, temp_overrun = false, pres_overrun = false;
  int error;

  temperature_error_ = 0;
  pressure_error_ = 0;
  temperature_valid_ = false;
  pressure_valid_ = false;
  instance_measurement_count_++;
  device_data_->read_total_++;

  if (device_data_ == nullptr) {
    return ENODEV;
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);  /* get the device lock
                                                     * device lock will be unlcoked when
                                                     * guard's destruct routine gets called
                                                     */
  /*
   * Start a measurement by sending a one shot command
   * We need to read in control register 2 and set the one shot bit.
   * Then write it back.
   */
  retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbCtrlReg2,
                                             &ctrl_register_2,
                                             sizeof(ctrl_register_2));
  if (retval != 0) {
    /*
     * If we got an error it applies to both the temperature and pressure
     */
    temperature_error_ = retval;
    pressure_error_ = retval;
    return retval;
  }
  ctrl_register_2 |= kLps22hbCtrlReg2OneShotMask;
  error = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg2,
                                          &ctrl_register_2,
                                          sizeof(ctrl_register_2));
  if (error != 0) {
        /*
     * If we got an error it applies to both the temperature and pressure
     */
    temperature_error_ = retval;
    pressure_error_ = retval;
    return error;
  }

  /*
   * Add a sleep to allow the command to execute on the device
   */
  usleep(10000);

  /*
   * Now loop on the status register waiting for data to be available
   */
  for (temp_available_count = 0; temp_available_count < 10;
       temp_available_count++) {

    /*
     * Get the status register
     */
    retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbStatus,
                                             &data_available,
                                             sizeof(data_available));
    if (retval != 0) {
      /*
       * Since this loops we may have gotten a valid value for temperature or pressure
       * on a previous pass. So, we only count an error for a value that is not marked
       * as valid.
       */
      if (temperature_valid_ == false) {
        temperature_error_ = retval;
      }
      if (pressure_valid_ == false) {
        pressure_error_ = retval;
      }
      return retval;
    }

    /*
     * Check for overruns
     * An overrun means that a new value has overwritten a previous one.
     * Since we are doing one-shots I don't see this as a problem. We
     * only want this new value we don't care if an older value has been
     * overwritten.
     * We record it, but don't really use it.
     */
    if ((data_available & kLps22hbStatusTemperatureDataOverRunMask) == kLps22hbStatusTemperatureDataOverRunMask) {
      temp_overrun = true;
    }
    if ((data_available & kLps22hbStatusPressureDataOverRunMask) == kLps22hbStatusPressureDataOverRunMask) {
      pres_overrun = true;
    }

    /*
     * If there is pressure data ready then get the pressure data
     */
    if ((pres_updated == false ) && ((data_available & kLps22hbStatusPressureDataAvailableMask) ==
        kLps22hbStatusPressureDataAvailableMask)) {
      error = i2cbus_.transferDataFromRegisters(
          slave_address_, kLps22hbPressureOutXl, pres_buffer,
          sizeof(pres_buffer));
      if (error != 0) {
        /*
         * Return whatever error was found at the lower area
         */
        pressure_error_ = error;
        return error;
      }

      pressure_measurement_ = ((pres_buffer[2] & 0x7f) << 16) |
                              pres_buffer[1] << 8 | pres_buffer[0];
      if ((pres_buffer[2] & 0x80) == 0x80) {
        pressure_measurement_ *= -1;
      }
      pres_updated = true;
      pressure_error_ = 0;
      pressure_valid_ = true;
      pressure_measurement_time_ = system_clock::now();
    }

    /*
     * If there is temperature available ready get the temperature data
     */
    if ((temp_updated == false) && ((data_available & kLps22hbStatusTemperatureDataAvailableMask) ==
        kLps22hbStatusTemperatureDataAvailableMask)) {
      error = i2cbus_.transferDataFromRegisters(
          slave_address_, kLps22hbTempOutL, temp_buffer, sizeof(temp_buffer));
      if (error != 0) {
        /*
         * Return whatever error was found at the lower area
         */
        temperature_error_ = error;
        return error;
      }
      temperature_measurement_ =
          ((temp_buffer[1] & 0x7F) << 8) | temp_buffer[0];
      if ((temp_buffer[1] & 0x80) == 0x80) {
        temperature_measurement_ *= -1;
      }
      temp_updated = true;
      temperature_valid_ = true;
      temperature_error_ = 0;
      temperature_measurement_time_ = system_clock::now();
    }

    if ((temp_updated == true) && (pres_updated == true)) {
      break;
    }
    /*
     * TODO: Need to get the correct value and make it a constant
     */
    usleep(10000);
  }

  last_read_ = steady_clock::now();

  return 0;
}

expected<TemperatureMeasurement, int> Lps22::getTemperatureMeasurement(TemperatureUnit_t unit) {
  uint8_t buffer[2] = {0, 0};
  float temperature;
  int error;

  if (device_data_ == nullptr) {
    return unexpected(ENODEV);
  }
  if (measurementExpired(temperature_last_read_)) {
    /*
     * The device seems to always return a temperature of 0 Centigrade
     * on the first read after a power cycle.
     * So, if this is the first read after a power cycle get another measurment
     */
    error = getMeasurement();
    if (device_data_->read_total_ == 1) {
      error = getMeasurement();
    }
  }

  if (temperature_valid_ == false) {
    return unexpected(temperature_error_);
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
      temperature = TemperatureDatum::celsiusToFahrenheit(temperature);
      break;
    case TEMPERATURE_UNIT_CELSIUS:
      /* It is already in celsius so just return it*/
      break;
    case TEMPERATURE_UNIT_KELVIN:
      temperature = TemperatureDatum::celsiusToKelvin(temperature);
      break;
  }

  TemperatureDatum data(temperature, unit);
  
  TemperatureMeasurement measurement(data, temperature_measurement_time_);

  return measurement;
}

expected<PressureMeasurement, int> Lps22::getPressureMeasurement(PressureUnit_t unit) {
  uint8_t buffer[3] = {0, 0, 0};
  float pressure;
  int error;

  if (device_data_ == nullptr) {
    return unexpected(ENODEV);
  }
  if (measurementExpired(pressure_last_read_) == true) {
    error = getMeasurement();
    if (pressure_valid_ == false) {
      return unexpected(pressure_error_);
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
      pressure = PressureDatum::MbToPa(pressure);
      break;
    case PRESSURE_UNIT_PSI:
      pressure = PressureDatum::MbToPsi(pressure);
      break;
    case PRESSURE_UNIT_INCHES_MERCURY:
      pressure = PressureDatum::MbToInchesMercury(pressure);
      break;
  }

  PressureDatum pdata(pressure, unit);

  PressureMeasurement measurement(pdata, pressure_measurement_time_);

  return measurement;
}

/*
 * Private methods
 */

bool Lps22::measurementExpired(time_point<steady_clock> steady_time) {
  /*
   * check if the current measurement has expired
   */
  if (instance_measurement_count_ == 0) {
    return true;
  }
  time_point<steady_clock> now = steady_clock::now();

  auto time_diff = duration_cast<milliseconds>(now - steady_time);

  if (time_diff > measurement_interval_) {
    return true;
  }

  return false;
}
