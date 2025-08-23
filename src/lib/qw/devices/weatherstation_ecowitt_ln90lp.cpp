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

#include <chrono>
#include <string>
#include <string_view>

#include "modbus/modbus-rtu.h"
#include "modbus/modbus.h"

#include "qw/devices/include/CRC.h"

#include "qw/units/humidity/include/relative_humidity.h"
#include "qw/units/humidity/include/relative_humidity_measurement.h"
#include "qw/units/pressure/include/millibar.h"
#include "qw/units/pressure/include/pressure_measurement.h"
#include "qw/units/speed/include/meters_per_second.h"
#include "qw/units/speed/include/speed_measurement.h"
#include "qw/units/temperature/include/celsius.h"
#include "qw/units/temperature/include/temperature.h"
#include "qw/units/temperature/include/temperature_measurement.h"

using qw::units::Celsius;
using qw::units::MetersPerSecond;
using qw::units::Millibar;
using qw::units::PressureMeasurement;
using qw::units::RelativeHumidityMeasurement;
using qw::units::SpeedMeasurement;
using qw::units::Temperature;
using qw::units::TemperatureMeasurement;
using std::expected;
using std::string;
using std::string_view;
using std::unexpected;
using std::chrono::milliseconds;
using std::chrono::system_clock;

namespace qw::units {

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
      TemperatureMeasurement(Temperature(), Temperature(), epoch);
  last_rh_ = RelativeHumidityMeasurement(RelativeHumidity(), RelativeHumidity(),
                                         epoch);
  last_pressure_ = PressureMeasurement(Pressure(), Pressure(), epoch);
  last_wind_speed_ = SpeedMeasurement(Speed(), Speed(), epoch);
}

WeatherStationEcowittLn90lp::~WeatherStationEcowittLn90lp() {}

expected<TemperatureMeasurement, int>
WeatherStationEcowittLn90lp::getTemperature() {
  /*
   * Check if we have gotten the temperature within the valid time frame
   */
  if ((system_clock::now() - last_temperature_.time()) <=
      temperature_valid_interval_) {
    return last_temperature_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   */
  expected<TemperatureMeasurement, int> new_temperature = readTemperatureData();
  if (new_temperature.has_value() != true) {
    return unexpected(new_temperature.error());
  }
  last_temperature_ = new_temperature.value();

  return new_temperature;
}

expected<RelativeHumidityMeasurement, int>
WeatherStationEcowittLn90lp::getRelativeHumidity() {
  /*
   * Check if we have gotten the relative humidity within the valid time frame
   */
  if ((system_clock::now() - last_rh_.time()) <= rh_valid_interval_) {
    return last_rh_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   */
  expected<RelativeHumidityMeasurement, int> new_rh =
      readRelativeHumidityData();
  if (new_rh.has_value() != true) {
    return unexpected(new_rh.error());
  }
  last_rh_ = new_rh.value();

  return new_rh;
}

expected<PressureMeasurement, int> WeatherStationEcowittLn90lp::getPressure() {
  /*
   * Check if we have gotten the pressure within the valid time frame
   */
  if ((system_clock::now() - last_pressure_.time()) <=
      pressure_valid_interval_) {
    return last_pressure_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   */
  expected<PressureMeasurement, int> new_pressure = readPressureData();
  if (new_pressure.has_value() != true) {
    return unexpected(new_pressure.error());
  }
  last_pressure_ = new_pressure.value();

  return new_pressure;
}

expected<SpeedMeasurement, int> WeatherStationEcowittLn90lp::getWindspeed() {
  /*
   * Check if we have gotten the wind speed within the valid time frame
   */
  if ((system_clock::now() - last_pressure_.time()) <=
      pressure_valid_interval_) {
    return last_wind_speed_;
  }
  /*
   * It has been too long since we last got the data so go get the data from the sensor
   */
  expected<SpeedMeasurement, int> new_wind_speed = readWindSpeedData();
  if (new_wind_speed.has_value() != true) {
    return unexpected(new_wind_speed.error());
  }
  last_wind_speed_ = new_wind_speed.value();

  return new_wind_speed;
}

expected<TemperatureMeasurement, int>
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

  TemperatureMeasurement tm(temperature.value(), kWsEwLn90lpTemperatureAccuracy,
                            system_clock::now());
  /*
   * Since we got a temperature data load it in the private variable
   */
  last_temperature_ = tm;

  return tm;
}

expected<RelativeHumidityMeasurement, int>
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

  RelativeHumidityMeasurement rhm(
      rh.value(), kWsEwLn90lpRelativeHumidityAccuracy, system_clock::now());

  /*
   * Since we got a relative humidity data load it in the private variable
   */
  last_rh_ = rhm;

  return rhm;
}

expected<PressureMeasurement, int>
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
      (pressure.value() > kWsEwLn90lpPressureRange[0])) {
    return unexpected(ERANGE);
  }

  PressureMeasurement pm(pressure.value(), kWsEwLn90lpPressureAccuracy,
                         system_clock::now());
  /*
   * Since we got a pressure data load it in the private variable
   */
  last_pressure_ = pm;

  return pm;
}

expected<SpeedMeasurement, int>
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

  MetersPerSecond ws_accuracy(1);
  if (wspd.value() >= MetersPerSecond(10)) {
    ws_accuracy = wspd.value() * .1;
  }

  if ((wspd.value() < kWsEwLn90lpWindSpeedRange[0]) ||
      (wspd.value() > kWsEwLn90lpWindSpeedRange[1])) {
    return unexpected(ERANGE);
  }

  SpeedMeasurement wspdm(wspd.value(), ws_accuracy, system_clock::now());
  /*
   * Since we got a temperature data load it in the private variable
   */
  last_wind_speed_ = wspdm;

  return wspdm;
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

  if ((temp_c < kWsEwLn90lpTemperatureRange[0]) ||
      (temp_c > kWsEwLn90lpTemperatureRange[1])) {
    return unexpected(ERANGE);
  }

  return temp_c;
}

expected<RelativeHumidity, int>
WeatherStationEcowittLn90lp::convertRawRelativeHumidityData(uint16_t raw_data) {
  if (raw_data == 0xFFFF) {
    return unexpected(EINVAL);
  }

  RelativeHumidity rhm(raw_data);

  if ((rhm < kWsEwLn90lpRelativeHumidityRange[0]) ||
      (rhm > kWsEwLn90lpRelativeHumidityRange[1])) {
    return unexpected(ERANGE);
  }

  return rhm;
}

expected<Pressure, int> WeatherStationEcowittLn90lp::convertRawPressureData(
    uint16_t raw_data) {
  if (raw_data == 0xFFFF) {
    return unexpected(EINVAL);
  }

  Millibar pressure(raw_data / 10);

  if ((pressure < kWsEwLn90lpPressureRange[0]) ||
      (pressure > kWsEwLn90lpPressureRange[1])) {
    return unexpected(ERANGE);
  }

  return pressure;
}

expected<Speed, int> WeatherStationEcowittLn90lp::convertRawWindSpeedData(
    uint16_t raw_data) {
  if (raw_data == 0xFFFF) {
    return unexpected(EINVAL);
  }

  MetersPerSecond wspd(raw_data / 10);

  if ((wspd < kWsEwLn90lpWindSpeedRange[0]) ||
      (wspd > kWsEwLn90lpWindSpeedRange[1])) {
    return unexpected(ERANGE);
  }

  return wspd;
}

expected<uint32_t, int> WeatherStationEcowittLn90lp::getBaudRate() {
  uint16_t speed_offset;

  int result =
      downloadModBusData(kWsEwLn90lpRtuRegisterDataRate, 1, &speed_offset);
  if (result != 0) {
    return unexpected(result);
  }

  if (speed_offset > kWsEwLn90lpBaudRateCount) {
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

int WeatherStationEcowittLn90lp::setBaudRate(uint32_t speed) {
  uint16_t baud_offset;

  for (baud_offset = 0; baud_offset < kWsEwLn90lpBaudRateCount; baud_offset++) {
    if (kWsEwLn90lpBaudRates[baud_offset] == speed) {
      break;
    }
  }

  if (baud_offset == kWsEwLn90lpBaudRateCount) {
    return EINVAL;
  }

  int result =
      uploadModBusData(kWsEwLn90lpRtuRegisterDataRate, 1, &baud_offset);

  if (result != 1) {
    return errno;
  }

  /*
   * If we successfully set the devioes baud rate set our local value to match.
   */
  baud_rate_ = kWsEwLn90lpBaudRates[baud_offset];

  return 0;
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

int WeatherStationEcowittLn90lp::setDeviceAddress(uint16_t device_address) {

  if ((device_address < kWsEwLn90lpAddressMin) ||
      (device_address > kWsEwLn90lpAddressMax)) {
    return EINVAL;
  }

  int result =
      uploadModBusData(kWsEwLn90lpRtuRegisterDeviceAddress, 1, &device_address);

  if (result != 1) {
    return errno;
  }

  /*
   * We successfully changed the address on the device so change it here.
   */
  slave_addr_ = device_address;

  return 0;
}

std::expected<uint32_t, int> WeatherStationEcowittLn90lp::getDeviceId() {
  uint16_t data[2];

  int result = downloadModBusData(kWsEwLn90lpRtuRegisterDeviceAddress, 1, data);
  if (result != 0) {
    return unexpected(result);
  }

  /*
   * The MSB is at offset 0 and the LSB at offset 1
   */
  uint32_t id = data[0] << 16;
  id |= data[1];

  return id;
}

expected<struct WsEwLn90lpSpecialDataResponse, int>
WeatherStationEcowittLn90lp::specialCommand(uint32_t baud_rate, uint8_t address) {
  struct WsEwLn90lpSpecialFrame inquiry;
  uint8_t bps;

  if (baud_rate != 0) {
    uint8_t i;
    for (i = 0; i < kWsEwLn90lpBaudRateCount; ++i) {
      if (kWsEwLn90lpBaudRates[i] ==  baud_rate) {
        break;
      }
    }
    if (i == kWsEwLn90lpBaudRateCount) {
      return unexpected(EINVAL);
    }
    bps = i + 1;
  } else {
    bps = 0;
  }
  if ((address < kWsEwLn90lpAddressMin) || (address > kWsEwLn90lpAddressMax)) {
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
  if (frame.data.bps < 1 || frame.data.bps > kWsEwLn90lpBaudRateCount) {
    return unexpected(EBADE);
  }
  data.baud_rate = kWsEwLn90lpBaudRates[frame.data.bps];
  data.device_address = frame.data.device_address;

  /*
   * We have the data we want so clen things up and return with it
   */
  modbus_close(ctx);
  modbus_free(ctx);

  return data;
}

}  // namespace qw::units
