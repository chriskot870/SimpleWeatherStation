/*
 * The main program to show the data.
 * Right now it's pretty simple
 */
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

#include "sht4x.h"
#include "lps22.h"

using namespace std;

constexpr char sht4x_path[] = "/sys/bus/i2c/drivers/sht4x/1-0044/hwmon/hwmon2";
constexpr char sht4x_temperature_path[] = "/sys/bus/i2c/drivers/sht4x/1-0044/hwmon/hwmon2/temp1_input";
constexpr char sht4x_humidity_path[] = "/sys/bus/i2c/drivers/sht4x/1-0044/hwmon/hwmon2/humidity1_input";

int main() {
    ifstream tempfh, humidityfh;
    string temperature;
    string humidity;
    float ctemp, pressure, hum, ftemp, sht44temp, lps22temp;
    string time_string;
    
    

    Lps22 lps22("/dev/i2c-1", kLps22hbI2cAddress);

    lps22.init();

    uint8_t whoami;
    whoami = lps22.whoami();
    printf("LPS22HB who am I Value: 0x%x\n", whoami);

    /*
     * The sht4x device is connected to I2c bus 1 at the primary address
     */
    I2cSht4x sht4x("/dev/i2c-1", kSht4xI2cPrimaryAddress);

    uint32_t serial_number;

    sht4x.softReset();
    if (sht4x.getSerialNumber(&serial_number) == false) {
        printf("Getting SHT44 Serial Number failed\n");
        exit(1);
    }
    printf("SHT44 Serial Number: %d\n", serial_number);

    cout << "Starting" << endl;
    while(true) {
        auto now_time = std::chrono::system_clock::now();
        auto cur_time = std::chrono::system_clock::to_time_t(now_time);
        std::cout << "Date: " << std::ctime(&cur_time);

        sht44temp = sht4x.getTemperature(TEMPERATURE_UNIT_CELSIUS);
        hum = sht4x.getRelativeHumidity();
        printf("SHT44 Raw Temperature Centigrade: %5.2f\n", sht44temp);
        printf("SHT44 Raw Temperature Fahrenheit: %5.2f\n", TemperatureConversion::celsiusToFahrenheit(sht44temp));
        printf("SHT44 Raw Relative Humidity: %5.2f\n", hum);

        lps22temp = lps22.getTemperature(TEMPERATURE_UNIT_CELSIUS);
        pressure = lps22.getBarometricPressure(PRESSURE_UNIT_INCHES_MERCURY);
        printf("LPS22 Raw Temperature Centigrade: %5.2f\n", lps22temp);
        printf("LPS22 Raw Temperature Fahrenheit: %5.2f\n", TemperatureConversion::celsiusToFahrenheit(lps22temp));
        printf("LPS22 Raw Barometric Pressure: %5.2f\n", pressure);

        cout << endl;
        sleep(10);
    }
}
