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

bool I2cSht4x::getSerialNumber(uint32_t *serial_number) {
    int32_t retval;
    int i2c_bus;
    uint8_t command = kSht4xCommandReadSerialNumber;
    uint8_t read_buffer[kSht4xSerialReturnLength] = {kSht4xCommandReadSerialNumber,0,0,0,0,0};
    struct i2c_msg fetch_serial_com;
    struct i2c_rdwr_ioctl_data xfer;

   /*
    * The serial number shouldn't change so we only have to get it once.
    * Check to see if all elements of the private serial number are 0.
    * If so then it means we haven't read the serial number before.
    * So, read it now.
    */

    if ( serial_number_ != 0) {
        *serial_number = serial_number_;
        return true;
    }

    /*
     * If I get here then I need to fetch the serial number
     */
    i2c_bus = open("/dev/i2c-1", O_RDWR);
    if ( i2c_bus < 0) {
        return false;
    }

    /*
     * Write the serial number command to the address
     */
    fetch_serial_com.addr = kSht4xI2cAddress;
    fetch_serial_com.flags = 0;
    fetch_serial_com.len = kSht4xCommandLength;
    fetch_serial_com.buf = &command;

    xfer.msgs = &fetch_serial_com;
    xfer.nmsgs = 1;

    retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
    if (retval < 0) {
        close(i2c_bus);
        return false;
    }

    /*
     * I seem to need a delay here
     */
    sleep(1);
    /*
     * Perform the read to get the serial number
     */
    fetch_serial_com.addr = kSht4xI2cAddress;
    fetch_serial_com.flags = I2C_M_RD;
    fetch_serial_com.len = kSht4xSerialReturnLength;
    fetch_serial_com.buf = read_buffer;

    retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
    if (retval < 0) {
        close(i2c_bus);
        return false;
    }
    sleep(1);

    /*
     * We need figure out how to check the CRC's
     */
    uint32_t serial_number_ = (read_buffer[0] << 24) +
                            (read_buffer[1] << 16) +
                            (read_buffer[3] << 8) +
                            read_buffer[4];

    close(i2c_bus);

    *serial_number = serial_number_;

    return true;
}
bool I2cSht4x::softReset() {
    int32_t retval;
    int i2c_bus;
    uint8_t command = kSht4xCommandReset;
    struct i2c_msg fetch_serial_com;
    struct i2c_rdwr_ioctl_data xfer;

    /*
     * If I get here then I need to fetch the serial number
     */
    i2c_bus = open("/dev/i2c-1", O_RDWR);
    if ( i2c_bus < 0) {
        return false;
    }

    
    fetch_serial_com.addr = kSht4xI2cAddress;
    fetch_serial_com.flags = 0;
    fetch_serial_com.len = kSht4xCommandLength;
    fetch_serial_com.buf = &command;

    xfer.msgs = &fetch_serial_com;
    xfer.nmsgs = 1;

    retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
    if (retval != 1) {
        close(i2c_bus);
        return false;
    }
    sleep(1);
    
    close(i2c_bus);

    return true;
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
    uint8_t measurement[] = {0, 0, 0, 0, 0, 0};
    struct i2c_msg fetch_serial_com;
    struct i2c_rdwr_ioctl_data xfer;
    uint8_t command = sht3x_measurement_command_map[mode];
    uint8_t read_buffer[] = {0,0,0,0,0,0};

    i2c_bus = open("/dev/i2c-1", O_RDWR);
    if ( i2c_bus < 0) {
        return;
    }

    /*
     * Write the command to get a measurement
     */
    fetch_serial_com.addr = kSht4xI2cAddress;
    fetch_serial_com.flags = 0;  /* Do a write */
    fetch_serial_com.len = kSht4xCommandLength;
    fetch_serial_com.buf = &command;

    xfer.msgs = &fetch_serial_com;
    xfer.nmsgs = 1;

    retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
    if (retval < 0) {
        return;
    }
    sleep(1);
    /*
     * Perform the read to get the measurement
     */
    fetch_serial_com.addr = kSht4xI2cAddress;
    fetch_serial_com.flags = I2C_M_RD;
    fetch_serial_com.len = kSht4xSerialReturnLength;
    fetch_serial_com.buf = read_buffer;

    retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
    if (retval < 0) {
        return;
    }
    sleep(1);
    close(i2c_bus);

    /*
     * We need figure out how to check the CRC's
     */
    temperature_measurement_ = (read_buffer[0] << 8) + read_buffer[1];
    relative_humidity_measurement_ = (read_buffer[3] << 8) + read_buffer[4];

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

    /* For now just return true */
    return true;

    if (elapsed_time > measurement_time_) {
        return true;
    }

    return false;
}

