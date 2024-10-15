/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * The main program to show the data.
 * Right now it's pretty simple
 */
#include <unistd.h>
#include <iostream>
#include <string>

#include "include/lps22.h"
#include "include/sht4x.h"

using std::cout;
using std::endl;
using std::string;

int main() {
  string temperature;
  string humidity;
  float ctemp, pressure, hum, ftemp, sht44temp, lps22temp;
  string time_string;
  std::expected<uint8_t, int> x_whoami;
  std::expected<uint32_t, int> x_serial_number;
  std::expected<float, int> x_sht4x_temp;
  std::expected<float, int> x_sht4x_humidity;
  std::expected<float, int> x_lps22_temp;
  std::expected<float, int> x_lps22_pressure;
  int error;

  I2cBus i2c_bus = I2cBus(string("/dev/i2c-1"));

  Lps22 lps22(i2c_bus, kLps22hbI2cAddress);

  error = lps22.init();
  if (error != 0) {
    printf("Initialization of lps22: %d\n", error);
    exit(1);
  }

  x_whoami = lps22.whoami();
  if (x_whoami.has_value() != true) {
    printf("Couldn't get Who am I value for lps22hb: %d\n", x_whoami.error());
    exit(1);
  }
  printf("LPS22HB who am I Value: 0x%x\n", x_whoami.value());

  /*
   * The sht4x device is connected to I2c bus 1 at the primary address
   */
  I2cSht4x sht4x(i2c_bus, kSht4xI2cPrimaryAddress);

  error = sht4x.softReset();
  if (error != 0) {
    printf("CHT4X reset failed/n");
    exit(1);
  }
  x_serial_number = sht4x.getSerialNumber();
  if (x_serial_number.has_value() == false) {
    printf("Getting SHT44 Serial Number failed: %d\n", x_serial_number.error());
    exit(1);
  }
  printf("SHT44 Serial Number: %d\n", x_serial_number.value());

  cout << "Starting" << endl;
  while (true) {
    auto now_time = std::chrono::system_clock::now();
    auto cur_time = std::chrono::system_clock::to_time_t(now_time);
    std::cout << "Date: " << std::ctime(&cur_time);

    x_sht4x_temp = sht4x.getTemperature(TEMPERATURE_UNIT_CELSIUS);
    if (x_sht4x_temp.has_value() == false) {
      printf("Failed to get Temperature from SHT4x Device: %d\n",
             x_sht4x_temp.error());
    }
    x_sht4x_humidity = sht4x.getRelativeHumidity();
    if (x_sht4x_humidity.has_value() == false) {
      printf("Failed to get Temperature from SHT4x Device: %d\n",
             x_sht4x_humidity.error());
    }
    printf("SHT44 Raw Temperature Centigrade: %5.2f\n", x_sht4x_temp.value());
    printf("SHT44 Raw Temperature Fahrenheit: %5.2f\n",
           TemperatureConversion::celsiusToFahrenheit(x_sht4x_temp.value()));
    printf("SHT44 Raw Relative Humidity: %5.2f\n", x_sht4x_humidity.value());

    x_lps22_temp = lps22.getTemperature(TEMPERATURE_UNIT_CELSIUS);
    if (x_lps22_temp.has_value() == false) {
      printf("Failed to get Temperature from LPS22HB Device: %d\n",
             x_lps22_temp.error());
      exit(1);
    }
    x_lps22_pressure =
        lps22.getBarometricPressure(PRESSURE_UNIT_Mb);
    if (x_lps22_pressure.has_value() == false) {
      printf("Failed to get Barometric Pressure from LPS22HB Device: %d\n",
             x_lps22_pressure.error());
      exit(1);
    }
    printf("LPS22 Raw Temperature Centigrade: %5.2f\n", x_lps22_temp.value());
    printf("LPS22 Raw Temperature Fahrenheit: %5.2f\n",
           TemperatureConversion::celsiusToFahrenheit(x_lps22_temp.value()));
    printf("LPS22 Raw Barometric Pressure millibar: %5.2f\n", x_lps22_pressure.value());
    printf("LPS22 Raw Barometric Pressure inches  : %5.2f\n", PressureConversion::MbTohInchesMercury(x_lps22_pressure.value()));

    cout << endl;
    sleep(10);
  }
}
