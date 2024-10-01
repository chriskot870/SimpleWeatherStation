/*
 * The main program to show the data.
 * Right now it's pretty simple
 */
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

#include "sht4x.h"

using namespace std;

constexpr char sht4x_path[] = "/sys/bus/i2c/drivers/sht4x/1-0044/hwmon/hwmon2";
constexpr char sht4x_temperature_path[] = "/sys/bus/i2c/drivers/sht4x/1-0044/hwmon/hwmon2/temp1_input";
constexpr char sht4x_humidity_path[] = "/sys/bus/i2c/drivers/sht4x/1-0044/hwmon/hwmon2/humidity1_input";

int main() {
    ifstream tempfh, humidityfh;
    string temperature;
    string humidity;
    float ctemp, hum, ftemp;

    
    I2cSht4x sht4x;
    uint32_t serial_number;

    sht4x.softReset();
    if (sht4x.getSerialNumber(&serial_number) == false) {
        printf("Getting Serial Number failed\n");
        exit(1);
    }
    printf("Serial Number: 0x%x\n", serial_number);

    ctemp = sht4x.getTemperature(TEMPERATURE_UNIT_CELSIUS);
    hum = sht4x.getRelativeHumidity();

    cout << "Starting" << endl;
    while(true) {
        ctemp = sht4x.getTemperature(TEMPERATURE_UNIT_CELSIUS);
        hum = sht4x.getRelativeHumidity();
        printf("Raw Temperature Centigrade: %5.2f\n", ctemp);
        printf("Raw Temperature Fahrenheit: %5.2f\n", TemperatureConversion::celsiusToFahrenheit(ctemp));
        printf("Raw Relative Humidity: %5.2f\n", hum);
    	tempfh.open(string(sht4x_temperature_path), fstream::in);
    	if (tempfh.is_open()) {
            getline(tempfh, temperature);
            ctemp = atof(&temperature[0])/1000;
            ftemp = ((9*ctemp)/5)+32;
            printf("%5.2fF %5.2fC\n", ftemp, ctemp);
            tempfh.close();
        } else {
            cout << "sht4x temperature unavailable\n";
        }

        humidityfh.open(string(sht4x_humidity_path), fstream::in);
        if (humidityfh.is_open()) {
            getline(humidityfh, humidity);
            hum = atof(&humidity[0])/1000;
            printf("%5.2f%%\n", hum);
            humidityfh.close();
        } else {
            cout << "sht4x humidity unavailable\n";
        }
        cout << endl;
        sleep(10);
    }
}
