/*
 * Copyright 2024,2025, Chris Kottaridis. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * NTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef SRC_LIB_QW_DEVICES_INCLUDE_WEATHERSTATION_ECOWITT_LN90LP_H_
#define SRC_LIB_QW_DEVICES_INCLUDE_WEATHERSTATION_ECOWITT_LN90LP_H_

#include <expected>

#include <array>
#include <string>
#include <string_view>

#include "modbus/modbus-rtu.h"
#include "modbus/modbus.h"

#include "qw/units/direction/include/degrees.h"
#include "qw/units/direction/include/direction_measurement.h"
#include "qw/units/humidity/include/relative_humidity.h"
#include "qw/units/humidity/include/relative_humidity_measurement.h"
#include "qw/units/pressure/include/millibar.h"
#include "qw/units/pressure/include/pressure_measurement.h"
#include "qw/units/speed/include/meters_per_second.h"
#include "qw/units/speed/include/speed_measurement.h"
#include "qw/units/temperature/include/celsius.h"
#include "qw/units/temperature/include/temperature_measurement.h"

namespace qw::devices {

constexpr std::string_view kWsEwLn90lpRtuDevice = "/dev/ttyS0";
constexpr uint kWsEwLn90lpRtuDeviceDefaultBaudRate = 9600;
constexpr char kWsEwLn90lpRtuDeviceParity = 'N';
constexpr uint8_t kWsEwLn90lpRtuDeviceDataBits = 8;
constexpr uint8_t kWsEwLn90lpRtuDeviceStopBits = 1;
constexpr uint8_t kWsEwLn90lpRtuDefaultSlaveAddress = 0x90;
// I assume this is a fixed value
// I came up with it by reading the register.
// I hope that it identifies the model, but I am not sure
constexpr uint16_t kWsEwLn90lpRtuDeviceId = 0x90;

// Special command code
constexpr uint8_t kWsEwLn90lpSpecialCommandCode[3] = {0xFD, 0xFD, 0xFD};

struct WsEwLn90lpSpecialDataInquiry {
  uint8_t bps;
  uint8_t device_address;
};

struct WsEwLn90lpSpecialDataResponse {
  uint32_t baud_rate;
  uint8_t device_address;
};

struct WsEwLn90lpSpecialFrame {
  uint8_t prefix[3];
  struct WsEwLn90lpSpecialDataInquiry data;
};

constexpr uint8_t kWsEwLn90lpErrIllegalFunction = 1;
constexpr uint8_t kWsEwLn90lpErrIllegalAddress = 2;
constexpr uint8_t kWsEwLn90lpErrIllegalData = 3;
constexpr uint8_t kWsEwLn90lpErrCrcFail = 8;

// One more than the offsets are the values to exchange with the device
// to get the corresponding baud rate.
// If when asked for the baud rate it returns a value of 2 then that
// means the value is at 2-1 offset. That would be 9600. To set the
// baud rate to 115200 you would send 3 + 1.
constexpr uint8_t kWsEwLn90lpBaudRateCount = 4;
constexpr std::array<uint32_t, 4> kWsEwLn90lpBaudRates({4800, 9600, 19200,
                                                        115200});

constexpr uint16_t kWsEwLn90lpAddressMin = 1;
constexpr uint16_t kWsEwLn90lpAddressMax = 252;

constexpr uint8_t kWsEwLn90lpRtuRead = 0x03;
constexpr uint8_t kWsEwLn90lpRtuWrite = 0x06;

// Temperature measuring range
const qw::units::Celsius kWsEwLn90lpTemperatureRange[2] = {
    qw::units::Celsius(-40), qw::units::Celsius(60)};
// +/-1 degree Celsius accuracy
const qw::units::Celsius kWsEwLn90lpTemperatureAccuracy(1);
// .1 degree Celsius resolution
const qw::units::Celsius kWsEwLn90lpTemperatureResolution(.1);

// Relative Humidity measuring range
const qw::units::RelativeHumidity kWsEwLn90lpRelativeHumidityRange[2] = {
    qw::units::RelativeHumidity(1), qw::units::RelativeHumidity(99)};
// +/-5 % Relative Humidity accuracy
const qw::units::RelativeHumidity kWsEwLn90lpRelativeHumidityAccuracy(5);
// 1% Relative Humidity resolution
const qw::units::RelativeHumidity kWsEwLn90lpRelativeHumidityResolution(1);

// Pressure measuring range
const qw::units::Millibar kWsEwLn90lpPressureRange[2] = {
    qw::units::Millibar(300), qw::units::Millibar(1100)};
// +/-5 mbar accuracy
const qw::units::Millibar kWsEwLn90lpPressureAccuracy(5);
// .1 mbar resolution
const qw::units::Millibar kWsEwLn90lpPressureResolution(.1);

// Wind speed measuring range
const qw::units::MetersPerSecond kWsEwLn90lpWindSpeedRange[2] = {
    qw::units::MetersPerSecond(0), qw::units::MetersPerSecond(40)};
// wind speed accuracy is not a constant. It is implemented in readWindSpeed().
// wind speed reolution
const qw::units::MetersPerSecond kWsEwLn90lpWindSpeedResolution(.1);

// Wind direction measuring range
const qw::units::Degrees kWsEwLn90lpWindDirectionRange[2] = {
    qw::units::Degrees(0), qw::units::Degrees(359)};
// +/- 15 degrees accuracy
const qw::units::Degrees kWsEwLn90lpWindDirectionAccuracy(15);
// 1 degree resolution
const qw::units::Degrees kWsEwLn90lpWindDirectionResolution(1);

constexpr uint16_t kWsEwLn90lpRtuRegisterDeviceName = 0x0160;
constexpr uint16_t kWsEwLn90lpRtuRegisterDataRate = 0x0161;
constexpr uint16_t kWsEwLn90lpRtuRegisterDeviceAddress = 0x0162;
constexpr uint16_t kWsEwLn90lpRtuRegisterDeviceIdMsb = 0x0163;
constexpr uint16_t kWsEwLn90lpRtuRegisterDeviceIdLsb = 0x0164;
constexpr uint16_t kWsEwLn90lpRtuRegisterLight = 0x0165;
constexpr uint16_t kWsEwLn90lpRtuRegisterUvi = 0x0166;
constexpr uint16_t kWsEwLn90lpRtuRegisterTemperature = 0x0167;
constexpr uint16_t kWsEwLn90lpRtuRegisterHumidity = 0x0168;
constexpr uint16_t kWsEwLn90lpRtuRegisterWindSpeed = 0x0169;
constexpr uint16_t kWsEwLn90lpRtuRegisterGustSpeed = 0x016A;
constexpr uint16_t kWsEwLn90lpRtuRegisterWindDirection = 0x016B;
constexpr uint16_t kWsEwLn90lpRtuRegisterRainfall = 0x016C;
constexpr uint16_t kWsEwLn90lpRtuRegisterAbsPressure = 0x016D;
constexpr uint16_t kWsEwLn90lpRtuRegisterRainCounter = 0x016E;

constexpr int kWsEwLn90lpRtuAllDataCount =
    kWsEwLn90lpRtuRegisterAbsPressure - kWsEwLn90lpRtuRegisterLight + 1;

/*
 * These command initiate a measurement
 */
constexpr uint16_t kWsEwLn90lpRtuMeasuringLight = 0x9C92;
constexpr std::chrono::milliseconds kWsEwLn90lpRtuMeasuringLightDelay(113);
constexpr uint16_t kWsEwLn90lpRtuMeasuringUvi = 0x9C93;
constexpr std::chrono::milliseconds kWsEwLn90lpRtuMeasuringUviDelay(113);
constexpr uint16_t kWsEwLn90lpRtuMeasuringTemperature = 0x9C94;
constexpr std::chrono::milliseconds kWsEwLn90lpRtuMeasuringTemperatureDelay(31);
constexpr uint16_t kWsEwLn90lpRtuMeasuringHumidity = 0x9C95;
constexpr std::chrono::milliseconds kWsEwLn90lpRtuMeasuringHumidityDelay(31);
constexpr uint16_t kWsEwLn90lpRtuMeasuringWindSpeed = 0x9C96;
constexpr std::chrono::milliseconds kWsEwLn90lpRtuMeasuringWindSpeedDelay(31);
constexpr uint16_t kWsEwLn90lpRtuMeasuringGustSpeed = 0x9C97;
constexpr std::chrono::milliseconds kWsEwLn90lpRtuMeasuringGustSpeedDelay(31);
constexpr uint16_t kWsEwLn90lpRtuMeasuringWindDirection = 0x9C98;
constexpr std::chrono::milliseconds kWsEwLn90lpRtuMeasuringWindDirectionDelay(
    31);
// constexpr uint16_t kWsEwLn90lpRtuReserved = 0x9C99;
constexpr uint16_t kWsEwLn90lpRtuMeasuringAbsPressure = 0x9C9A;
constexpr std::chrono::milliseconds kWsEwLn90lpRtuMeasuringWindPressureDelay(
    136);

struct WsEwLn90lpRtuInputData {
  uint16_t light;
  uint16_t uvi;
  uint16_t temperature;
  uint16_t humidity;
  uint16_t wind_speed;
  uint16_t gust_speed;
  uint16_t wind_direction;
  uint16_t rainfall;
};

// The device updates it's values every 8.8 seconds.
constexpr std::chrono::milliseconds kWsEwLn90lpDataRefreshInterval(8800);
// Wind speed is faster every 2.2 seconds
constexpr std::chrono::milliseconds kWsEwLn90lpWindSpeedDataRefreshInterval(
    2200);

class WeatherStationEcowittLn90lp {
 public:
  explicit WeatherStationEcowittLn90lp(std::string_view device_name);

  bool initialize(uint32_t baud = 0, uint8_t device_addr = 0);

  /*
   * These three routines make a thermometer
   */
  std::expected<qw::units::TemperatureMeasurement, int> getTemperature();

  std::chrono::milliseconds getTemperatureValidInterval();

  void setTemperatureValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a hygrometer
   */
  std::expected<qw::units::RelativeHumidityMeasurement, int>
  getRelativeHumidity();

  std::chrono::milliseconds getRelativeHumidityValidInterval();

  void setRelativeHumidityValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a barometer
   */
  std::expected<qw::units::PressureMeasurement, int> getPressure();

  std::chrono::milliseconds getPressureValidInterval();

  void setPressureValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make an anemometer
   */
  std::expected<qw::units::SpeedMeasurement, int> getWindspeed();

  std::chrono::milliseconds getWindSpeedValidInterval();

  void setWindSpeedValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a wind vane
   */
  std::expected<qw::units::DirectionMeasurement, int> getWindDirection();

  std::chrono::milliseconds getWindDirectionValidInterval();

  void setWindDirectionValidInterval(std::chrono::milliseconds interval);

  /*
   * THhse are the unbuffered read routines called by the get routines above.
   * These actually go fetch the data from the device.
   */
  std::expected<qw::units::TemperatureMeasurement, int> readTemperatureData();

  std::expected<qw::units::RelativeHumidityMeasurement, int>
  readRelativeHumidityData();

  std::expected<qw::units::PressureMeasurement, int> readPressureData();

  std::expected<qw::units::SpeedMeasurement, int> readWindSpeedData();

  std::expected<qw::units::DirectionMeasurement, int> readWindDirectionData();

  /*
   * MIicellaneous control functions
   */
  uint32_t getLocalBaudRate();

  std::expected<uint32_t, int> getDeviceBaudRate();

  int setLocalBaudRate(uint32_t baud_rate);

  int setDeviceBaudRate(uint32_t speed);

  uint8_t getSlaveAddress();

  std::expected<uint8_t, int> getDeviceAddress();

  int setSlaveAddress(uint8_t address);

  int setDeviceAddress(uint16_t device_address);

  std::expected<uint16_t, int> getDeviceId();

  std::expected<struct WsEwLn90lpSpecialDataResponse, int> specialCommand(
      uint32_t baud_rate, uint8_t address);

  bool findAndMatchDevice();

  ~WeatherStationEcowittLn90lp();

 private:
  std::string_view device_name_;
  uint baud_rate_;      // Initially 9600 can be changed
  char parity_;         // Fixed at N
  uint8_t data_bits_;   // Fixed at 8
  uint8_t stop_bits_;   // Fixed at 1
  uint8_t slave_addr_;  // Initially 0x90 can be changed on device
  WsEwLn90lpRtuInputData buffer_;

  qw::units::TemperatureMeasurement last_temperature_;
  std::chrono::milliseconds temperature_valid_interval_ =
      kWsEwLn90lpDataRefreshInterval;

  qw::units::RelativeHumidityMeasurement last_rh_;
  std::chrono::milliseconds rh_valid_interval_ = kWsEwLn90lpDataRefreshInterval;

  qw::units::PressureMeasurement last_pressure_;
  std::chrono::milliseconds pressure_valid_interval_ =
      kWsEwLn90lpDataRefreshInterval;

  qw::units::SpeedMeasurement last_wind_speed_;
  std::chrono::milliseconds wind_speed_valid_interval_ =
      kWsEwLn90lpDataRefreshInterval;

  qw::units::DirectionMeasurement last_wind_direction_;
  std::chrono::milliseconds wind_direction_valid_interval_ =
      kWsEwLn90lpDataRefreshInterval;

  int downloadModBusData(uint16_t addr, int count, uint16_t* buffer);

  int uploadModBusData(uint16_t addr, int count, uint16_t* buffer);

  std::expected<qw::units::Temperature, int> convertRawTemperatureData(
      uint16_t raw_data);

  std::expected<qw::units::RelativeHumidity, int>
  convertRawRelativeHumidityData(uint16_t raw_data);

  std::expected<qw::units::Pressure, int> convertRawPressureData(
      uint16_t raw_data);

  std::expected<qw::units::Speed, int> convertRawWindSpeedData(
      uint16_t raw_data);
};

}  // namespace qw::devices

#endif  // SRC_LIB_QW_DEVICES_INCLUDE_WEATHERSTATION_ECOWITT_LN90LP_H_
