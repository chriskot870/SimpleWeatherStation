/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains the SHT45 information. See data sheet
 * https://cdn-shop.adafruit.com/product-files/5665/5665_Datasheet_SHT4x.pdf
 *
 */

#ifndef SRC_LIB_DEVICES_I2C_INCLUDE_LPS22_H_
#define SRC_LIB_DEVICES_I2C_INCLUDE_LPS22_H_

#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <chrono>
#include <string>
#include <expected>

#include "pressure_interface.h"
#include "temperature_interface.h"

using std::expected;
using std::unexpected;

constexpr std::chrono::milliseconds kLps22DefaultMeasurementInterval(
    2000); /* The number of msecs that a reading is good */
/*
 * Fixed address. could be 0x55 you have to check the model from the data sheet
 */
constexpr uint8_t kLps22hbI2cAddress =
    0x5D;  // or 0x5C, See data sheet section 7.2.1.

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

class Lps22 : public TemperatureInterface, public BarometricPressureInterface {
 public:
  Lps22(std::string i2cbus_name, uint8_t slave_address);

  uint8_t deviceAddress();

  int init();

  expected<uint8_t, int> whoami();

  int getMeasurement();

  expected <float, int> getTemperature(TemperatureUnit_t unit);

  expected <float, int> getBarometricPressure(PressureUnit_t unit);

  std::chrono::milliseconds getMeasurementInterval();

  int setMeasurementInterval(std::chrono::milliseconds interval);

 private:
  /*
   * Private Variables
   */

  // i2c bus device name
  std::string i2cbus_name_;
  uint8_t slave_address_;

  uint64_t measurement_count_ = 0;

  // minimum interval between making a measurement.
  std::chrono::milliseconds measurement_interval_ =
      kLps22DefaultMeasurementInterval;  // Interval between measurements

  // The last time point a measurement was made. Initialize to zero.
  std::chrono::time_point<std::chrono::steady_clock> last_read_{};

  /*
   * The temperature is a 16 bit signed value
   */
  int16_t temperature_measurement_ = 0;

  /*
   * The barometric pressure is a signed 24 bit value so we treat it as a 32 bit signed value
   */
  int32_t pressure_measurement_ = 0;

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
  expected <uint8_t, int> getRegister(uint8_t reg);

  int setRegister(uint8_t reg, uint8_t value);

  int getRegisters(uint8_t reg, uint8_t* data, uint8_t count);

  int setRegisters(uint8_t reg, uint8_t* data, uint8_t count);

  bool measurementExpired();
};

#endif  // SRC_LIB_DEVICES_I2C_INCLUDE_LPS22_H_
