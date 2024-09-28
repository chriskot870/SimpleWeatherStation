/*
 * This contains the driver for the SHT4x series. Specifically the SHT45
 */

#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <chrono>
#include <cstring>
#include <algorithm>

#include "sht4x.h"

uint8_t I2cSht4x::deviceAddress() {

    return kSht4xI2cAddress;
}

bool I2cSht4x::getSerialNumber(Sht4xSerialNumber_t *serial_number) {
    int32_t retval;
    int i2c_bus;
    uint8_t command = kSht4xCommandReadSerialNumber;

   /*
    * The serial number shouldn't change so we only have to get it once.
    * Check to see if all elements of the private serial number are 0.
    * If so then it means we haven't read the serial number before.
    * So, read it now.
    */

    for( auto i : serial_number_) {
        if ( i != 0) {
            memcpy(serial_number, serial_number_, sizeof(Sht4xSerialNumber_t));
            return true;
        }
    }

    /*
     * If I get here then I need to fetch the serial number
     */
    i2c_bus = open("/dev/i2c-1", O_RDWR);
    if ( i2c_bus != 0) {
        return false;
    }

    /*
     * Select the slave address
     */
    if (ioctl(i2c_bus, I2C_SLAVE, kSht4xI2cAddress)) {
        close(i2c_bus);
        return false;
    }

    i2c_smbus_read_i2c_block_data(i2c_bus, kSht4xCommandReset,sizeof(Sht4xSerialNumber_t),  serial_number_);

    close(i2c_bus);

    memcpy(serial_number, serial_number_, sizeof(Sht4xSerialNumber_t));

    return true;
}

void I2cSht4x::softReset() {
    int32_t retval;
    int i2c_bus;

    /*
     * If I get here then I need to fetch the serial number
     */
    i2c_bus = open("/dev/i2c-1", O_RDWR);
    if ( i2c_bus != 0) {
        return;
    }

    /*
     * Select the slave address
     */
    if (ioctl(i2c_bus, I2C_SLAVE, kSht4xI2cAddress)) {
        close(i2c_bus);
        return;
    }

    /*
     * Send the reset command
     */
    retval = i2c_smbus_write_quick(i2c_bus, kSht4xCommandReset);

    close(i2c_bus);

    return;
}

float I2cSht4x::getTemperature(TemperatureUnit_t unit) {
    float temperature;

    if (measurementExpired() == true) {
        getMeasurement(SHT4X_MEASUREMENT_PRECISION_HIGH);
    }

    switch(unit) {
        case TEMPERATURE_UNIT_FAHRENHEIT :
            temperature = (kSht4xTemperatureFahrenheitSlope * (float)temperature_measurement_) - kSht4xTemperatureFahrenheitOffset;
            break;
        case TEMPERATURE_UNIT_CELSIUS :
            temperature = (kSht4xTemperatureCelsiusSlope * (float)temperature_measurement_) - kSht4xTemperatureCelsiusOffset;
            break;
        case TEMPERATURE_UNIT_KELVIN :
            temperature = (kSht4xTemperatureKelvinSlope * (float)temperature_measurement_) - kSht4xTemperatureKelvinOffset;
            break;
    }

    return temperature;
}

timeslice I2cSht4x::getMeasurementInterval() {

    return measurement_time_;
}

void I2cSht4x::setMeasurementInterval(timeslice interval) {

    measurement_time_ = interval;

    return;
}

float I2cSht4x::getRelativeHumidity() {
    float relative_humidity;

    if (measurementExpired() == true) {
        getMeasurement(SHT4X_MEASUREMENT_PRECISION_HIGH);
    }

    relative_humidity = (kSht4xRelativeHumiditySlope * (float)relative_humidity_measurement_) - kSht4xRelativeHumidityOffset;

    /*
     * SHT4x document says:
     * cropping of the RH signal to the range of 0 %RH … 100 %RH is advised.
     */
    if (relative_humidity < 0.0) {
        return 0.0;
    }
    if (relative_humidity > 100.0) {
        return 100.0;
    }

    return relative_humidity;
}

/*
 * Private Methods
 */
void I2cSht4x::getMeasurement(Sht4xMeasurmentMode mode) {
    int retval;
    int i2c_bus;
    uint8_t command;
    uint8_t measurement[] = {0, 0, 0, 0, 0, 0};

    command = sht3x_measurement_command_map[mode];

    i2c_bus = open("/dev/i2c-1", O_RDWR);
    if ( i2c_bus != 0) {
        return;
    }

    /*
     * Select the slave address
     */
    if (ioctl(i2c_bus, I2C_SLAVE, kSht4xI2cAddress)) {
        close(i2c_bus);
        return;
    }
    i2c_smbus_read_i2c_block_data(i2c_bus, command, sizeof(measurement), measurement);

    /*
     * One read returns both the temperature and the relative humidity
     * So, we have to break out each value and store them in the private variables
     */
    /*
    short *temp_ptr, temp;
    temp_ptr = (short *)&measurement[3];
    temp = *temp_ptr;
    */
    temperature_measurement_ = (measurement[kSht4xDataTemperatureMsbOffset] << 8) | measurement[kSht4xDataTemperatureLsbOffset];
    /*
    short *hum_ptr, hum;
    hum_ptr = (short *)&measurement[0];
    hum = *hum_ptr;
     */
    relative_humidity_measurement_ = (measurement[kSht4xDataRelativeHumidityMsbOffset] << 8) | measurement[kSht4xDataRelativeHumidityLsbOffset];

    last_read_ = std::chrono::steady_clock::now();

    return;
}

bool I2cSht4x::measurementExpired() {

    /*
     * check if the current measurement has expired
     * If time of measurement is zero then no measurement has been
     * taken so count it as expired.
     * Make the zero check early so if it is true we don't need to bother calling
     * any functions to get the current time.
     */
    /*
    if (time_of_measurement_ms_ == 0) {
        return true;
    }
    */

   auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_read_);

    if (elapsed_time > measurement_time_) {
        return true;
    }

    return false;
}

