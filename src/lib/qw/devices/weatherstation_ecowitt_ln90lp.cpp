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

/*
  * This is a driver for the Ecowitt WN90LP weatherstation. It communicates
  * via RS485 using the modbus prototcol. See
  * https://shop.ecowitt.com/products/wn90lp
  * The manuals can be found here
  * https://www.ecowitt.com/api/quickstart/product?id=287
  * 
  * It provides data for the following weather properties:
  * Temperature
  * Humidity
  * Barometric Pressure
  * Rainfall
  * Wind Speed
  * Gust Speed
  * Wind Direction
  * Light Metering
  * UV Metering
  */

#include "qw/devices/include/weatherstation_ecowitt_ln90lp.h"

#include <expected>

#include <algorithm>
#include <array>
#include <chrono>
#include <string>
#include <string_view>

#include "modbus/modbus-rtu.h"
#include "modbus/modbus.h"

#include "qw/units/humidity/include/relative_humidity.h"
#include "qw/units/pressure/include/millibar.h"
#include "qw/units/pressure/include/pressure.h"
#include "qw/units/speed/include/meters_per_second.h"
#include "qw/units/speed/include/speed.h"
#include "qw/units/temperature/include/celsius.h"
#include "qw/units/temperature/include/temperature.h"
#include "qw/units/direction/include/degrees.h"
#include "qw/units/direction/include/direction.h"
#include "qw/units/uvi/include/uvi.h"
#include "qw/units/include/unit_measurement.h"

using qw::units::Millibar;
using qw::units::Pressure;
using qw::units::RelativeHumidity;
using qw::units::MetersPerSecond;
using qw::units::Speed;
using qw::units::Degrees;
using qw::units::Direction;
using qw::units::Celsius;
using qw::units::Temperature;
using qw::units::Uvi;
using qw::units::UnitMeasurement;
using std::expected;
using std::find;
using std::string;
using std::string_view;
using std::unexpected;
using std::chrono::milliseconds;
using std::chrono::system_clock;

namespace qw::devices {

WeatherStationEcowittLn90lp::WeatherStationEcowittLn90lp(
    string_view device_name)
    : device_name_(device_name),
      baud_rate_(
          kWsEwLn90lpRtuDeviceDefaultBaudRate),  // Set to default baud rate
      parity_(kWsEwLn90lpRtuDeviceParity),  // This value is fixed on the device
      data_bits_(
          kWsEwLn90lpRtuDeviceDataBits),  // This value is fixed on the device
      stop_bits_(
          kWsEwLn90lpRtuDeviceStopBits),  // This value is fixed on the device
      slave_addr_(kWsEwLn90lpRtuDefaultSlaveAddress) {
  /*
   * Initialize with essentially NULL data. MOst important thing is the epoch.
   * The time of the last measurement is checked to see if it is longer than
   * the data refresh interval. The epoch chould be set to Jan 1 1970. That
   * should be longer than the refresh interval on first pass. So, on first
   * pass it will go try to download the data.
   */
  system_clock::time_point epoch;
  last_temperature_ =
      UnitMeasurement<Temperature>(Temperature(), Temperature(), epoch);
  last_rh_ = UnitMeasurement<RelativeHumidity>(RelativeHumidity(), RelativeHumidity(),
                                         epoch);
  last_pressure_ = UnitMeasurement<Pressure>(Pressure(), Pressure(), epoch);
  last_wind_speed_ = UnitMeasurement<Speed>(Speed(), Speed(), epoch);
  last_wind_direction_ = UnitMeasurement<Direction>(Direction(), Direction(), epoch);
}

WeatherStationEcowittLn90lp::~WeatherStationEcowittLn90lp() {}

bool WeatherStationEcowittLn90lp::initialize(uint32_t baud,
                                             uint8_t device_addr) {
  // Find baud rate and address of the device and set local values to match them
  if (findAndMatchDevice() != true) {
    return false;
  }
  // Sanity check the parameters before making any changes
  auto it = kWsEwLn90lpBaudRates.begin();
  for (it; it != kWsEwLn90lpBaudRates.end(); ++it) {
    if (*it == baud) {
      break;
    }
  }
  if (it == kWsEwLn90lpBaudRates.end()) {
    return false;
  }

  if ((device_addr != 0) && ((device_addr < kWsEwLn90lpAddressMin) ||
                             device_addr > kWsEwLn90lpAddressMax)) {
    return false;
  }
  /*
   * The values are valid. If I want to change one of them use specialCommand
   */
  if (baud != 0 || device_addr != 0) {
    std::expected<struct WsEwLn90lpSpecialDataResponse, int> result =
        specialCommand(baud, device_addr);
    if (result.has_value() != true) {
      return false;
    }
    /*
     * Makesure to have the local baud ad address match what the device reported
     */
    baud_rate_ = result.value().baud_rate;
    slave_addr_ = result.value().device_address;
  }
  return true;
}

expected<UnitMeasurement<Temperature>, int>
WeatherStationEcowittLn90lp::getTemperature() {
  /*
   * Check if we have gotten the temperature within the valid time frame
   */
  if ((system_clock::now() - last_temperature_.timeStamp()) <=
      temperature_valid_interval_) {
    return last_temperature_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   * readTemperatureData() will update the last_temperature_ value.
   */
  expected<UnitMeasurement<Temperature>, int> x_new_temperature = readTemperatureData();
  if (x_new_temperature.has_value() != true) {
    return unexpected(x_new_temperature.error());
  }

  last_temperature_ = x_new_temperature.value();

  return last_temperature_;
}

milliseconds WeatherStationEcowittLn90lp::getTemperatureValidInterval() {
  return temperature_valid_interval_;
}

void WeatherStationEcowittLn90lp::setTemperatureValidInterval(std::chrono::milliseconds interval) {
  temperature_valid_interval_ = interval;

  return;
}

expected<UnitMeasurement<RelativeHumidity>, int>
WeatherStationEcowittLn90lp::getRelativeHumidity() {
  /*
   * Check if we have gotten the relative humidity within the valid time frame
   */
  if ((system_clock::now() - last_rh_.timeStamp()) <= rh_valid_interval_) {
    return last_rh_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   * The readRelativeHumidityData() will update last_rh_
   */
  expected<UnitMeasurement<RelativeHumidity>, int> x_new_rh =
      readRelativeHumidityData();
  if (x_new_rh.has_value() != true) {
    return unexpected(x_new_rh.error());
  }

  last_rh_ = x_new_rh.value();

  return last_rh_;
}

milliseconds WeatherStationEcowittLn90lp::getRelativeHumidityValidInterval() {
  return rh_valid_interval_;
}

void WeatherStationEcowittLn90lp::setRelativeHumidityValidInterval(milliseconds interval) {
  rh_valid_interval_ = interval;

  return;
}

expected<UnitMeasurement<Pressure>, int> WeatherStationEcowittLn90lp::getPressure() {
  /*
   * Check if we have gotten the pressure within the valid time frame
   */
  if ((system_clock::now() - last_pressure_.timeStamp()) <=
      pressure_valid_interval_) {
    return last_pressure_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   * The readPressureData() will update last_pressure_
   */
  expected<UnitMeasurement<Pressure>, int> x_new_pressure = readPressureData();
  if (x_new_pressure.has_value() != true) {
    return unexpected(x_new_pressure.error());
  }

  last_pressure_ = x_new_pressure.value();

  return last_pressure_;
}

milliseconds WeatherStationEcowittLn90lp::getPressureValidInterval() {
  return pressure_valid_interval_;
}

void WeatherStationEcowittLn90lp::setPressureValidInterval(std::chrono::milliseconds interval) {
  pressure_valid_interval_ = interval;

  return;
}

expected<UnitMeasurement<Speed>, int> WeatherStationEcowittLn90lp::getWindspeed() {
  /*
   * Check if we have gotten the wind speed within the valid time frame
   */
  if ((system_clock::now() - last_wind_speed_.timeStamp()) <=
      wind_speed_valid_interval_) {
    return last_wind_speed_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   * The readWindSpeedData() will update last_wind_speed_
   */
  expected<UnitMeasurement<Speed>, int> x_new_wind_speed = readWindSpeedData();
  if (x_new_wind_speed.has_value() != true) {
    return unexpected(x_new_wind_speed.error());
  }

  last_wind_speed_ = x_new_wind_speed.value();

  return last_wind_speed_;
}

milliseconds WeatherStationEcowittLn90lp::getWindSpeedValidInterval() {
  return wind_speed_valid_interval_;
}

void WeatherStationEcowittLn90lp::setWindSpeedValidInterval(milliseconds interval) {
  wind_speed_valid_interval_ = interval;

  return;
}

expected<UnitMeasurement<Direction>, int> WeatherStationEcowittLn90lp::getWindDirection() {
  /*
   * Check if we have gotten the wind direction within the valid time frame
   */
  if ((system_clock::now() - last_wind_direction_.timeStamp()) <=
      wind_direction_valid_interval_) {
    return last_wind_direction_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   * The readWindSpeedData() will update last_wind_speed_
   */
  expected<UnitMeasurement<Direction>, int> x_new_wind_direction = readWindDirectionData();
  if (x_new_wind_direction.has_value() != true) {
    return unexpected(x_new_wind_direction.error());
  }

  last_wind_direction_ = x_new_wind_direction.value();

  return last_wind_direction_;
}

milliseconds WeatherStationEcowittLn90lp::getWindDirectionValidInterval() {
  return wind_speed_valid_interval_;
}

void WeatherStationEcowittLn90lp::setWindDirectionValidInterval(milliseconds interval) {
  wind_direction_valid_interval_ = interval;

  return;
}

expected<UnitMeasurement<Uvi>, int> WeatherStationEcowittLn90lp::getUvi() {
  /*
   * Check if we have gotten the wind direction within the valid time frame
   */
  if ((system_clock::now() - last_uvi_.timeStamp()) <=
      uvi_valid_interval_) {
    return last_uvi_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   * The readWindSpeedData() will update last_wind_speed_
   */
  expected<UnitMeasurement<Uvi>, int> x_new_uvi = readUviData();
  if (x_new_uvi.has_value() != true) {
    return unexpected(x_new_uvi.error());
  }

  last_uvi_ = x_new_uvi.value();

  return last_uvi_;
}

milliseconds WeatherStationEcowittLn90lp::getUviValidInterval() {
  return uvi_valid_interval_;
}

void WeatherStationEcowittLn90lp::setUviValidInterval(milliseconds interval) {
  uvi_valid_interval_ = interval;

  return;
}

expected<UnitMeasurement<Temperature>, int>
WeatherStationEcowittLn90lp::readTemperatureData() {
  /*
   * Get the temperature data
   */
  uint16_t raw_temperature;
  int result = downloadModBusData(kWsEwLn90lpRtuRegisterTemperature, 1,
                                  &raw_temperature);
  if (result != 0) {
    return unexpected(result);
  }

  expected<Temperature, int> temperature =
      convertRawTemperatureData(raw_temperature);
  if (temperature.has_value() != true) {
    return unexpected(temperature.error());
  }

  if ((temperature.value() < kWsEwLn90lpTemperatureRange[0]) ||
      (temperature.value() > kWsEwLn90lpTemperatureRange[1])) {
    return unexpected(ERANGE);
  }

  UnitMeasurement<Temperature> tm(temperature.value(), kWsEwLn90lpTemperatureAccuracy,
                            system_clock::now());
  /*
   * Since we got a temperature data load it in the private variable
   */
  last_temperature_ = tm;

  return tm;
}

expected<UnitMeasurement<RelativeHumidity>, int>
WeatherStationEcowittLn90lp::readRelativeHumidityData() {
  /*
   * Get the relative humidity data
   */
  uint16_t raw_rh;
  int result = downloadModBusData(kWsEwLn90lpRtuRegisterHumidity, 1, &raw_rh);
  if (result != 0) {
    return unexpected(result);
  }

  expected<RelativeHumidity, int> rh = convertRawRelativeHumidityData(raw_rh);
  if (rh.has_value() != true) {
    return unexpected(rh.error());
  }

  if ((rh.value() < kWsEwLn90lpRelativeHumidityRange[0]) ||
      (rh.value() > kWsEwLn90lpRelativeHumidityRange[1])) {
    return unexpected(ERANGE);
  }

  UnitMeasurement<RelativeHumidity> rhm(
      rh.value(), kWsEwLn90lpRelativeHumidityAccuracy, system_clock::now());

  /*
   * Since we got a relative humidity data load it in the private variable
   */
  last_rh_ = rhm;

  return rhm;
}

expected<UnitMeasurement<Pressure>, int>
WeatherStationEcowittLn90lp::readPressureData() {
  /*
   * Get the pressure data
   */
  uint16_t raw_pressure;
  int result =
      downloadModBusData(kWsEwLn90lpRtuRegisterAbsPressure, 1, &raw_pressure);
  if (result != 0) {
    return unexpected(result);
  }

  expected<Pressure, int> pressure = convertRawPressureData(raw_pressure);
  if (pressure.has_value() != true) {
    return unexpected(pressure.error());
  }

  if ((pressure.value() < kWsEwLn90lpPressureRange[0]) ||
      (pressure.value() > kWsEwLn90lpPressureRange[1])) {
    return unexpected(ERANGE);
  }

  UnitMeasurement<Pressure> pm(pressure.value(), kWsEwLn90lpPressureAccuracy,
                         system_clock::now());
  /*
   * Since we got a pressure data load it in the private variable
   */
  last_pressure_ = pm;

  return pm;
}

expected<UnitMeasurement<Speed>, int>
WeatherStationEcowittLn90lp::readWindSpeedData() {
  /*
   * Get the wind speed data
   */
  uint16_t raw_wind_speed;
  int result =
      downloadModBusData(kWsEwLn90lpRtuRegisterWindSpeed, 1, &raw_wind_speed);
  if (result != 0) {
    return unexpected(result);
  }

  expected<Speed, int> wspd = convertRawWindSpeedData(raw_wind_speed);
  if (wspd.has_value() != true) {
    return unexpected(wspd.error());
  }

  MetersPerSecond mps = wspd.value();

  if ((mps < kWsEwLn90lpWindSpeedRange[0]) ||
      (mps > kWsEwLn90lpWindSpeedRange[1])) {
    return unexpected(ERANGE);
  }

  MetersPerSecond ws_accuracy(1);
  if (wspd.value() >= MetersPerSecond(10)) {
    ws_accuracy = wspd.value() * .1;
  }

  UnitMeasurement<Speed> wspdm(wspd.value(), ws_accuracy, system_clock::now());
  /*
   * Since we got a temperature data load it in the private variable
   */
  last_wind_speed_ = wspdm;

  return wspdm;
}

expected<UnitMeasurement<Direction>, int> WeatherStationEcowittLn90lp::readWindDirectionData() {
  /*
   * Get the wind direction data
   */
  uint16_t raw_wind_direction;
  int result =
      downloadModBusData(kWsEwLn90lpRtuRegisterWindDirection, 1, &raw_wind_direction);
  if (result != 0) {
    return unexpected(result);
  }

  /*
   * There is no conversion necessary. The returned value is 0 to 359.
   * So, just check the range
   */
  Degrees direct(raw_wind_direction);

  if ((direct < kWsEwLn90lpWindDirectionRange[0]) ||
      (direct > kWsEwLn90lpWindDirectionRange[1])) {
    return unexpected(ERANGE);
  }

  UnitMeasurement<Direction> wdm(direct, kWsEwLn90lpWindDirectionAccuracy, system_clock::now());
  /*
   * Since we got a wind direction data load it in the private variable
   */
  last_wind_direction_ = wdm;

  return last_wind_direction_;
}

expected<UnitMeasurement<Uvi>, int>
WeatherStationEcowittLn90lp::readUviData() {
  /*
   * Get the temperature data
   */
  uint16_t raw_uvi;
  int result = downloadModBusData(kWsEwLn90lpRtuRegisterUvi, 1,
                                  &raw_uvi);
  if (result != 0) {
    return unexpected(result);
  }

  expected<Uvi, int> x_uvi =
      convertRawUviData(raw_uvi);
  if (x_uvi.has_value() != true) {
    return unexpected(x_uvi.error());
  }

  Uvi uvi = x_uvi.value();

  if ((uvi < kWsEwLn90lpUviRange[0]) ||
      (uvi > kWsEwLn90lpUviRange[1])) {
    return unexpected(ERANGE);
  }

  UnitMeasurement<Uvi> uvim(uvi, kWsEwLn90lpUviAccuracy,
                            system_clock::now());
  /*
   * Since we got a temperature data load it in the private variable
   */
  last_uvi_ = uvim;

  return last_uvi_;
}

int WeatherStationEcowittLn90lp::downloadModBusData(uint16_t addr, int count,
                                                    uint16_t* buffer) {
  /*
   * We get all the data here in one request
   */
  modbus_t* ctx = modbus_new_rtu(device_name_.data(), baud_rate_, parity_,
                                 data_bits_, stop_bits_);
  if (ctx == nullptr) {
    return errno;
  }

  if (modbus_connect(ctx) == -1) {
    modbus_free(ctx);
    return errno;
  }

  /*
   * We want to use modbus_read_registers so it uses function code 0x03
   */
  int result = modbus_set_slave(ctx, slave_addr_);
  if (result != 0) {
    modbus_close(ctx);
    modbus_free(ctx);
    return errno;
  }
  result = modbus_read_registers(ctx, addr, count, buffer);

  if (result != count) {
    modbus_close(ctx);
    modbus_free(ctx);
    return errno;
  }

  modbus_close(ctx);
  modbus_free(ctx);

  return 0;
}

int WeatherStationEcowittLn90lp::uploadModBusData(uint16_t addr, int count,
                                                  uint16_t* buffer) {
  /*
   * We get all the data here in one request
   */
  modbus_t* ctx = modbus_new_rtu(device_name_.data(), baud_rate_, parity_,
                                 data_bits_, stop_bits_);
  if (ctx == nullptr) {
    return errno;
  }

  if (modbus_connect(ctx) == -1) {
    modbus_free(ctx);
    return errno;
  }

  /*
   * We want to use modbus_read_registers so it uses function code 0x03
   */
  int result = modbus_set_slave(ctx, slave_addr_);
  if (result != 0) {
    modbus_close(ctx);
    modbus_free(ctx);
    return errno;
  }
  result = modbus_write_registers(ctx, addr, count, buffer);

  if (result != count) {
    modbus_close(ctx);
    modbus_free(ctx);
    return errno;
  }

  modbus_close(ctx);
  modbus_free(ctx);

  return 0;
}

expected<Temperature, int>
WeatherStationEcowittLn90lp::convertRawTemperatureData(uint16_t raw_data) {
  if (raw_data == 0xFFFF) {
    return unexpected(EINVAL);
  }

  float temp = static_cast<float>(raw_data - 400) / 10;
  Celsius temp_c(temp);

  return temp_c;
}

expected<RelativeHumidity, int>
WeatherStationEcowittLn90lp::convertRawRelativeHumidityData(uint16_t raw_data) {
  if (raw_data == 0xFFFF) {
    return unexpected(EINVAL);
  }

  RelativeHumidity rhm(raw_data);

  return rhm;
}

expected<Pressure, int> WeatherStationEcowittLn90lp::convertRawPressureData(
    uint16_t raw_data) {
  if (raw_data == 0xFFFF) {
    return unexpected(EINVAL);
  }

  Millibar pressure(raw_data / 10);

  return pressure;
}

expected<Speed, int> WeatherStationEcowittLn90lp::convertRawWindSpeedData(
    uint16_t raw_data) {
  if (raw_data == 0xFFFF) {
    return unexpected(EINVAL);
  }

  MetersPerSecond wspd(raw_data / 10);

  return wspd;
}

expected<Uvi, int> WeatherStationEcowittLn90lp::convertRawUviData(
  uint16_t raw_data) {
  if (raw_data == 0xFFFF) {
    return unexpected(EINVAL);
  }

  Uvi uv_index(raw_data);

  return uv_index;

}


uint32_t WeatherStationEcowittLn90lp::getLocalBaudRate() {
  return baud_rate_;
}

expected<uint32_t, int> WeatherStationEcowittLn90lp::getDeviceBaudRate() {
  uint16_t speed_offset;

  int result =
      downloadModBusData(kWsEwLn90lpRtuRegisterDataRate, 1, &speed_offset);
  if (result != 0) {
    return unexpected(result);
  }

  if (speed_offset > kWsEwLn90lpBaudRates.size()) {
    /*
     * We got a bad value. Use Invalid Exchange EBADE. Seems like the best match
     */
    return unexpected(EBADE);
  }

  /*
   * Now that we have the offset return the corresponding value
   */

  uint32_t speed = kWsEwLn90lpBaudRates[speed_offset - 1];

  return speed;
}

int WeatherStationEcowittLn90lp::setLocalBaudRate(uint32_t baud_rate) {
  auto it =
      find(kWsEwLn90lpBaudRates.begin(), kWsEwLn90lpBaudRates.end(), baud_rate);
  if (it == kWsEwLn90lpBaudRates.end()) {
    return EINVAL;
  }
  baud_rate_ = baud_rate;

  return 0;
}

int WeatherStationEcowittLn90lp::setDeviceBaudRate(uint32_t speed) {
  uint16_t baud_offset;

  for (baud_offset = 0; baud_offset < kWsEwLn90lpBaudRates.size();
       baud_offset++) {
    if (kWsEwLn90lpBaudRates[baud_offset] == speed) {
      break;
    }
  }

  if (baud_offset == kWsEwLn90lpBaudRates.size()) {
    return EINVAL;
  }

  /*
   * There is a register that says it is RW to get and set the baud rate.
   * When I try to set the baud rate it fails. SO, use the special command
   * to set the baud rate.
   */
  std::expected<struct WsEwLn90lpSpecialDataResponse, int> result =
      specialCommand(speed, 0);
  if (result.has_value() != true) {
    return result.error();
  }

  /*
   * If we successfully set the devices baud rate set our local value to match.
   */
  baud_rate_ = speed;

  return 0;
}

uint8_t WeatherStationEcowittLn90lp::getSlaveAddress() {
  return slave_addr_;
}

expected<uint8_t, int> WeatherStationEcowittLn90lp::getDeviceAddress() {
  uint16_t address;

  int result =
      downloadModBusData(kWsEwLn90lpRtuRegisterDeviceAddress, 1, &address);
  if (result != 0) {
    return unexpected(result);
  }

  if ((address < kWsEwLn90lpAddressMin) || (address > kWsEwLn90lpAddressMax)) {
    return unexpected(EBADE);
  }

  return address;
}

int WeatherStationEcowittLn90lp::setSlaveAddress(uint8_t address) {
  if ((address < kWsEwLn90lpAddressMin) || (address > kWsEwLn90lpAddressMax)) {
    return EINVAL;
  }

  slave_addr_ = address;

  return 0;
}

int WeatherStationEcowittLn90lp::setDeviceAddress(uint16_t device_address) {
  if ((device_address < kWsEwLn90lpAddressMin) ||
      (device_address > kWsEwLn90lpAddressMax)) {
    return EINVAL;
  }

  /*
   * There is a register that says it is RW to get and set the address.
   * Baud rate also has a similar RW register, but it failed when I tried
   * to write it. Since I used special command for baud rate I'll use it
   * for address as well.
   */
  std::expected<struct WsEwLn90lpSpecialDataResponse, int> result =
      specialCommand(0, device_address);
  if (result.has_value() != true) {
    return result.error();
  }

  /*
   * We successfully changed the address on the device so change it here.
   */
  slave_addr_ = device_address;

  return 0;
}

std::expected<uint16_t, int> WeatherStationEcowittLn90lp::getDeviceId() {
  uint16_t data;

  int result = downloadModBusData(kWsEwLn90lpRtuRegisterDeviceAddress, 1, &data);
  if (result != 0) {
    return unexpected(result);
  }

  return data;
}

expected<struct WsEwLn90lpSpecialDataResponse, int>
WeatherStationEcowittLn90lp::specialCommand(uint32_t baud_rate,
                                            uint8_t address) {
  struct WsEwLn90lpSpecialFrame inquiry;
  uint8_t bps;

  if (baud_rate != 0) {
    uint8_t i;
    for (i = 0; i < kWsEwLn90lpBaudRates.size(); ++i) {
      if (kWsEwLn90lpBaudRates[i] == baud_rate) {
        break;
      }
    }
    if (i == kWsEwLn90lpBaudRates.size()) {
      return unexpected(EINVAL);
    }
    bps = i + 1;
  } else {
    bps = 0;
  }
  /*
   * Address == 0 means we are inquiring about the address. So it is valid
   */
  if ((address != 0) && ((address < kWsEwLn90lpAddressMin) ||
                         (address > kWsEwLn90lpAddressMax))) {
    return unexpected(EINVAL);
  }

  /*
   * Build the Special Command inquiry
   */
  inquiry.prefix[0] = 0xFD;
  inquiry.prefix[1] = 0xFD;
  inquiry.prefix[2] = 0xFD;
  inquiry.data.bps = bps;
  inquiry.data.device_address = address;

  modbus_t* ctx = modbus_new_rtu(device_name_.data(), baud_rate_, parity_,
                                 data_bits_, stop_bits_);
  if (ctx == nullptr) {
    return unexpected(errno);
  }

  if (modbus_connect(ctx) == -1) {
    modbus_free(ctx);
    return unexpected(errno);
  }

  // Make the inquiry
  int result =
      modbus_send_raw_request(ctx, reinterpret_cast<uint8_t*>(&inquiry),
                              sizeof(WsEwLn90lpSpecialFrame));
  if (result == -1) {
    modbus_close(ctx);
    modbus_free(ctx);
    return unexpected(errno);
  }

  /*
   * Create a buffer large enough to make sure you can handle any response
   */
  uint8_t* response = new uint8_t[MODBUS_RTU_MAX_ADU_LENGTH];
  // Get the reply
  result = modbus_receive_confirmation(ctx, response);
  if (result == -1) {
    delete response;
    modbus_close(ctx);
    modbus_free(ctx);
    return unexpected(errno);
  }

  struct WsEwLn90lpSpecialFrame* frame_buffer =
      reinterpret_cast<WsEwLn90lpSpecialFrame*>(response);
  struct WsEwLn90lpSpecialFrame frame = *frame_buffer;
  // We are done with the response so delete it
  delete response;

  struct WsEwLn90lpSpecialDataResponse data;
  if (frame.data.bps < 1 || frame.data.bps > kWsEwLn90lpBaudRates.size()) {
    return unexpected(EBADE);
  }
  data.baud_rate = kWsEwLn90lpBaudRates[frame.data.bps - 1];
  data.device_address = frame.data.device_address;

  /*
   * We have the data we want so clen things up and return with it
   */
  modbus_close(ctx);
  modbus_free(ctx);

  return data;
}

bool WeatherStationEcowittLn90lp::findAndMatchDevice() {
  /*
   * We use the special command to try and find the device's address and baud rate.
   * Of course we can't talk to the device if the baud rate isn't correct. So,
   * we have to walk through the different rates and call the special command
   * and see which one returns a success.
   * To increase our chances we start with the current baud_rate value
   */
  int32_t try_baud = getLocalBaudRate();
  uint8_t start_offset;
  for (int n = 0; n < kWsEwLn90lpBaudRates.size(); n++) {
    if (try_baud == kWsEwLn90lpBaudRates[n]) {
      start_offset = n;
      break;
    }
  }

  for (int cnt = 0, i = start_offset; cnt < kWsEwLn90lpBaudRates.size();
       i = ++i % kWsEwLn90lpBaudRates.size(), ++cnt) {
    setLocalBaudRate(kWsEwLn90lpBaudRates[i]);
    expected<struct WsEwLn90lpSpecialDataResponse, int> attempt =
        specialCommand(0, 0);
    if (attempt.has_value() == true) {
      /* It was a successful attempt so use the beaudrate and address*/
      if (setLocalBaudRate(attempt.value().baud_rate) != 0) {
        continue;
      }
      if (setSlaveAddress(attempt.value().device_address) != 0) {
        continue;
      }
      /*
       * If it gets here I got a result and loaded the values locally.
       * This means it was a success.
       */
      return true;
    }
  }

  /*
   * If we get here it means we couldn't find the device
   * Set the baud rate to the original value
   */
  setLocalBaudRate(kWsEwLn90lpBaudRates[start_offset]);

  return false;
}

}  // namespace qw::devices
