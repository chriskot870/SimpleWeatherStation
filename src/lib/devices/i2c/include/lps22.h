/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains the LPS22HB information. See data sheet
 * https://cdn-shop.adafruit.com/product-files/5665/5665_Datasheet_SHT4x.pdf
 *
 */

#ifndef SRC_LIB_DEVICES_I2C_INCLUDE_LPS22_H_
#define SRC_LIB_DEVICES_I2C_INCLUDE_LPS22_H_

#include <errno.h>
#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdatomic.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <expected>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

/*
 * This device provides temperature and pressure data so include the interfaces.
 * The device makes temperature and pressure measurements so add those includes.
 */
#include "pressure_interface.h"
#include "pressure_measurement.h"
#include "temperature_interface.h"
#include "temperature_measurement.h"

/*
 * This is an i2c bus device so add the i2cbus.h
 */
#include "include/i2cbus.h"

using std::expected;
using std::find;
using std::lock_guard;
using std::make_shared;
using std::map;
using std::mutex;
using std::recursive_mutex;
using std::shared_ptr;
using std::string;
using std::unexpected;
using std::vector;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using std::chrono::system_clock;
using std::chrono::time_point;

constexpr uint8_t kLps22ResetWaitCount = 10;

constexpr std::chrono::milliseconds kLps22MinimumMeasurementInterval(
    1000); /* The minimum value for measurement_interval_ */

constexpr milliseconds kLps22DefaultMeasurementInterval(
    2000); /* The number of msecs that a reading is good */

constexpr int kLps22WaitResponseLoopCount = 10;
/*
 * There are two possible slave addresses
 */
constexpr uint8_t kLps22hbI2cPrimaryAddress =
    0x5D;  // See data sheet section 7.2.1.
constexpr uint8_t kLps22hbI2cSecondaryAddress =
    0x5C;  // See data sheet section 7.2.1.

const vector<uint8_t> lps22_slave_address_options = {
    kLps22hbI2cPrimaryAddress, kLps22hbI2cSecondaryAddress};

/*
 * The constant value always retrurned by the Who Am I register
 */
constexpr uint8_t kLps22hbWhoAmIValue = 0xB1;  // This is the value for LPS22HB
constexpr int kLps22hbPressureHpaFactor =
    4096;  // convert measured value to hPa
constexpr int kLps22hbTemperatureFactor = 100;
constexpr uint8_t kLps22hbMaxRegistersTransferred =
    5;  // The maximum number of registers in one transfer

/*
 * Registers
 */
constexpr uint8_t kLps22hbInterruptCfg = 0x0B;
constexpr uint8_t kLps22hbThsPL = 0x0C;
constexpr uint8_t kLps22hbThsPH = 0x0D;

constexpr uint8_t kLps22hbWhoAmI = 0x0F;
constexpr uint8_t kLps22hbCtrlReg1 = 0x10;
constexpr uint8_t kLps22hbCtrlReg2 = 0x11;
constexpr uint8_t kLps22hbCtrlReg3 = 0x12;

constexpr uint8_t kLps22hbFifoCtrl = 0x14;
constexpr uint8_t kLps22hbRefPXL = 0x15;
constexpr uint8_t kLps22hbRefPL = 0x16;
constexpr uint8_t kLps22hbRefPH = 0x17;
constexpr uint8_t kLps22hbRpdsL = 0x18;
constexpr uint8_t kLps22hbRpdsH = 0x19;
constexpr uint8_t kLps22hbResConf = 0x1A;

constexpr uint8_t kLps22hbIntSource = 0x25;
constexpr uint8_t kLps22hbFifoStatus = 0x26;
constexpr uint8_t kLps22hbStatus = 0x27;
constexpr uint8_t kLps22hbPressureOutXl = 0x28;
constexpr uint8_t kLps22hbPressureOutL = 0x29;
constexpr uint8_t kLps22hbPressureOutH = 0x2A;
constexpr uint8_t kLps22hbTempOutL = 0x2B;
constexpr uint8_t kLps22hbTempOutH = 0x2C;

constexpr uint8_t kLps22hbLpfpRes = 0x33;

constexpr uint8_t kLps22hbResConfMaskLcEn = 0x1;

/*
 * Control Register 1 Info
 */
constexpr uint8_t kLps22hbCtrlReg1Default = 0x0;  // THe default is all values 0
// Single bit flags
constexpr uint8_t kLps22hbCtrlReg1SimMask = 0x01;
constexpr uint8_t kLps22hbCtrlReg1BduMask = 0x02;
constexpr uint8_t kLps22hbCtrlReg1LpfpCfgMask = 0x04;
constexpr uint8_t kLps22hbCtrlReg1EnLpfpMask = 0x08;

// Multiple bit flags
constexpr uint8_t kLps22hbCtrlReg1OdrMask = 0x7;
constexpr uint8_t kLps22hbCtrlReg1OdrShift = 4;

typedef enum {
  LPS22HB_CTRL_REG_1_ODR_POWER_DOWN,
  LPS22HB_CTRL_REG_1_ODR_1_HZ,
  LPS22HB_CTRL_REG_1_ODR_10_HZ,
  LPS22HB_CTRL_REG_1_ODR_25_HZ,
  LPS22HB_CTRL_REG_1_ODR_50_HZ,
  LPS22HB_CTRL_REG_1_ODR_75_HZ,
  LPS22HB_CTRL_REG_1_ODR_MODE_MAX
} Lps22hbOdr_t;

/*
 * Control Register 2 Info
 */
constexpr uint8_t kLps22hbCtrlReg2Default = 0x10;
// Single bit flags
constexpr uint8_t kLps22hbCtrlReg2OneShotMask = 0x01;
constexpr uint8_t kLps22hbCtrlReg2SwResetMask = 0x04;
constexpr uint8_t kLps22hbCtrlReg2I2cDisMask = 0x08;
constexpr uint8_t kLps22hbCtrlReg2IfAddIncMask = 0x10;
constexpr uint8_t kLps22hbCtrlReg2StopOnFthMask = 0x20;
constexpr uint8_t kLps22hbCtrlReg2FifoEnMask = 0x40;
constexpr uint8_t kLps22hbCtrlReg2BootMask = 0x80;

/*
 * Control Register 3 
 */
constexpr uint8_t kLps22hbCtrlReg3Default = 0x00;
// Single bit flags
constexpr uint8_t kLps22hbCtrlReg3DrdyMask = 0x04;
constexpr uint8_t kLps22hbCtrlReg3FOvrMask = 0x08;
constexpr uint8_t kLps22hbCtrlReg3FFthMask = 0x10;
constexpr uint8_t kLps22hbCtrlReg3FFss5Mask = 0x20;
constexpr uint8_t kLps22hbCtrlReg3PpOvdMask = 0x40;
constexpr uint8_t kLps22hbCtrlReg3IntHLMask = 0x80;
// Mulitple bit flags
constexpr uint8_t kLps22hbCtrlReg3IntSMask = 0x03;
constexpr uint8_t kLps22hbCtrlReg3IntSShift = 0x00;

typedef enum {
  LPS22HB_CTRL_REG_3_INT_S_DATA_SIGNAL,
  LPS22HB_CTRL_REG_3_INT_S_PRESSURE_HIGH,
  LPS22HB_CTRL_REG_3_INT_S_PRESSURE_LOW,
  LPS22HB_CTRL_REG_3_INT_S_PRESSURE_LOW_OR_HIGH
} Lps22hbIntS_t;

/*
 * Status Register Info
 */
// Single bit flags
constexpr uint8_t kLps22hbStatusPressureDataAvailableMask = 0x01;
constexpr uint8_t kLps22hbStatusTemperatureDataAvailableMask = 0x02;
constexpr uint8_t kLps22hbStatusPressureDataOverRunMask = 0x10;
constexpr uint8_t kLps22hbStatusTemperatureDataOverRunMask = 0x20;

/*
 * FIFO Control Register
 */
// Multiple bit flags
constexpr uint8_t kLps22hbCtrlRegFifoCtrlFModeMask = 0x7;
constexpr uint8_t kLps22hbCtrlRegFifoCtrlFModeShift = 5;
constexpr uint8_t kLps22hbCtrlRegFifoCtrlWTMMask = 0x1F;
constexpr uint8_t kLps22hbCtrlRegFifoCtrlWTMShift = 0;

/*
 * Control register default values
 */
constexpr uint8_t default_ctrl_reg_1_ = 0;
constexpr uint8_t default_ctrl_reg_2_ =
    kLps22hbCtrlReg2IfAddIncMask;  // So we can do multiple register reads

typedef enum {
  LPS22HB_CTRL_FIFO_CTRL_BYPASS_MODE,
  LPS22HB_CTRL_FIFO_CTRL_FIFO_MODE,
  LPS22HB_CTRL_FIFO_CTRL_STREAM_MODE,
  LPS22HB_CTRL_FIFO_CTRL_STREAM_TO_FIFO_MODE,
  LPS22HB_CTRL_FIFO_CTRL_BYPASS_TO_STREAM_MODE,
  LPS22HB_CTRL_FIFO_CTRL_RESERVED_MODE,
  LPS22HB_CTRL_FIFO_CTRL_DYNAMIC_STREAM_MODE,
  LPS22HB_CTRL_FIFO_CTRL_BYPASS_TO_FIFO_MODE
} Lps22hbFifoCtrl_t;

typedef enum { LPS22HB_TEMPERATURE, LPS22HB_PRESSURE } Lps22hbReading_t;

class Lps22DeviceLocation {
 public:
  string bus_name_;
  uint8_t slave_address_;

  /*
   * We need the == comparison to support the contain function for this class
   * to be used as a key in a map.
   */
  bool operator==(const Lps22DeviceLocation& data) const {
    if ((bus_name_ == data.bus_name_) &&
        (slave_address_ == data.slave_address_)) {
      return true;
    }
    return false;
  }

  /*
   * If you have == you should also have !=
   */
  bool operator!=(const Lps22DeviceLocation& data) const {
    if ((bus_name_ == data.bus_name_) &&
        (slave_address_ == data.slave_address_)) {
      return false;
    }
    return true;
  }

  /*
   * We need the < operator in order to use this class as a key for a map
   * We set the order that the busname is checked then slave on that bus
   */
  bool operator<(const Lps22DeviceLocation& data) const {
    if (bus_name_.compare(data.bus_name_) < 0)
      return true;
    if (bus_name_.compare(data.bus_name_) > 0)
      return false;
    if (slave_address_ < data.slave_address_)
      return true;
    return false;
  }
};

class Lps22DeviceData {
 public:
  std::recursive_mutex lock_ = {};
  uint64_t read_total_ = 0;
  atomic_bool initialized = false;

  /*
   * The time we read in the temperature
   */
  int16_t temperature_measurement_ = 0;
  time_point<system_clock> temperature_measurement_system_time_;
  time_point<steady_clock> temperature_measurement_steady_time_;
  milliseconds temperature_response_time;

  int32_t pressure_measurement_ = 0;
  time_point<system_clock> pressure_measurement_system_time_;
  time_point<steady_clock> pressure_measurement_steady_time_;
  milliseconds pressure_response_time;
};

class Lps22 : public TemperatureInterface, public BarometricPressureInterface {
 public:
  Lps22(I2cBus i2cbus_, uint8_t slave_address);

  int reset();

  int init();

  expected<uint8_t, int> whoami();

  expected<TemperatureMeasurement, int> getTemperatureMeasurement(
      TemperatureUnit_t unit);

  expected<PressureMeasurement, int> getPressureMeasurement(
      PressureUnit_t unit);

  milliseconds getMeasurementInterval(Lps22hbReading_t reading);

  int setMeasurementInterval(milliseconds interval, Lps22hbReading_t reading);

 private:
  /*
   * Private Variables
   */
  static mutex lps22_devices_lock;
  static map<Lps22DeviceLocation, shared_ptr<Lps22DeviceData>> lps22_devices;
  /*
   * There can be multiple instances of this class.
   */

  Lps22DeviceLocation
      device_;  // Where the device is located on the system, bus and slave
  shared_ptr<Lps22DeviceData> device_data_ = nullptr;
  I2cBus i2cbus_;          // The i2c bus used to transfer data
  uint8_t slave_address_;  // slave address for device on the bus
  atomic_uint64_t instance_measurement_count_ = 0;
  milliseconds temperature_interval_ = kLps22DefaultMeasurementInterval;
  milliseconds pressure_interval_ = kLps22DefaultMeasurementInterval;

  bool temperature_error_;
  bool temperature_valid_ = false;

  bool pressure_error_;
  bool pressure_valid_ = false;

  int error_code_ = 0;

  string error_message_ = {};

  /*
   * This is only going to be used for taking ambient temperature and
   * barometric temperature which doesn't change within a second. So,
   * we don't need to enable the high speed modes that take measurements
   * automatically every second or less.
   * So, we want to run in power down mode then use one shot to start a
   * measurment of both pressure and temperature. We have to check the status
   * bits to wait for the temperature and pressure to take their reading.
   * Then we can read both the temperature and pressure in one read.
   */
  const uint8_t default_ctrl_reg_1_ = 0;

  const uint8_t default_ctrl_reg_2_ =
      kLps22hbCtrlReg2IfAddIncMask;  // So we can do multiple register reads

  /*
    * Private Functions
    */

  int getMeasurement();

  bool measurementExpired(time_point<steady_clock> last_read_time,
                          milliseconds interval);
};

#endif  // SRC_LIB_DEVICES_I2C_INCLUDE_LPS22_H_
