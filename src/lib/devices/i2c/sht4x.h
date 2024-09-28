/*
 * This contains the SHT45 information. See data sheet
 * https://cdn-shop.adafruit.com/product-files/5665/5665_Datasheet_SHT4x.pdf
 *
 */

#ifndef LIB_DEVICES_I2C_SHT4X_I2C_H
#define LIB_DEVICES_I2C_SHT4X_I2C_H

#include <stdio.h>
#include <map>
#include <atomic>
#include "temperature_interface.h"
#include "relative_humidity_interface.h"

using timeslice = std::chrono::duration<int, std::ratio<1,1000>>;

/*
 * Fixed address. could be 0x45 you have to check the model from the data sheet
 */
constexpr uint8_t kSht4xI2cAddress = 0x44;

/*
 * Commands
 */
constexpr uint8_t kSht4xCommandLength = 1;
constexpr uint8_t kSht4xSerialLength = 6;
constexpr uint8_t kSht4xResetLength = 1;
constexpr uint8_t kSht4xReadDataLength = 6;
constexpr uint8_t kSht4xDataTemperatureMsbOffset = 0;
constexpr uint8_t kSht4xDataTemperatureLsbOffset = 1;
constexpr uint8_t kSht4xDataTemperatureCrcOffset = 2;
constexpr uint8_t kSht4xDataRelativeHumidityMsbOffset = 3;
constexpr uint8_t kSht4xDataRelativeHumidityLsbOffset = 4;
constexpr uint8_t kSht4xDataRelaiveHumidityCrcOffset = 5;

constexpr uint8_t kSht4xCommandHighPrecisionMeasurement = 0xFD;
constexpr uint8_t kSht4xCommandMediumPrecisionMeasurement = 0xF6;
constexpr uint8_t kSht4xCommandLowPrecisionMeasurement = 0xE0;
constexpr uint8_t kSht4xCommandReadSerialNumber = 0x89;
constexpr uint8_t kSht4xCommandReset = 0x94;
constexpr uint8_t kSht4xCommandActivateHtr200mwOneSecond = 0x39;
constexpr uint8_t kSht4xCommandActivateHtr200mwTenthSecond = 0x32;
constexpr uint8_t kSht4xCommandActivateHtr110mwOneSecond = 0x2F;
constexpr uint8_t kSht4xCommandActivateHtr110mwTenthSecond = 0x24;
constexpr uint8_t kSht4xCommandActivateHtr20mwOneSecond = 0x1E;
constexpr uint8_t kSht4xCommandActivateHtr20mwTenthSecond = 0x15;

constexpr float kSht4xTemperatureCelsiusSlope = ((float)175/(float)65535);
constexpr int kSht4xTemperatureCelsiusOffset = 45;
constexpr float kSht4xTemperatureFahrenheitSlope = ((float)315/(float)65535);
constexpr int kSht4xTemperatureFahrenheitOffset = 49;
constexpr float kSht4xRelativeHumiditySlope = ((float)125/(float)65535);
constexpr int kSht4xRelativeHumidityOffset = 6;
constexpr float kSht4xTemperatureKelvinSlope = kSht4xTemperatureCelsiusSlope;
constexpr float kSht4xTemperatureKelvinOffset = kSht4xTemperatureCelsiusOffset + 273.15;

typedef enum {
    SHT4X_MEASUREMENT_PRECISION_HIGH,
    SHT4X_MEASUREMENT_PRECISION_MEDIUM,
    SHT4X_MEASUREMENT_PRECISION_LOW,
    SHT4X_HEATER_ACTIVATION_200mW_ONE_SECOND,
    SHT4X_HEATER_ACTIVATION_200mW_TENTH_SECOND,
    SHT4X_HEATER_ACTIVATION_110mW_ONE_SECOND,
    SHT4X_HEATER_ACTIVATION_110mW_TENTH_SECOND,
    SHT4X_HEATER_ACTIVATION_20mW_ONE_SECOND,
    SHT4X_HEATER_ACTIVATION_20mW_TENTH_SECOND,
} Sht4xMeasurmentMode;

/*
 * The order here has to match the enum above so that you get the right value inthe array
 */
const uint8_t sht3x_measurement_command_map[] = {
    kSht4xCommandHighPrecisionMeasurement,
    kSht4xCommandMediumPrecisionMeasurement,
    kSht4xCommandLowPrecisionMeasurement ,
    kSht4xCommandActivateHtr200mwOneSecond,
    kSht4xCommandActivateHtr200mwTenthSecond,
    kSht4xCommandActivateHtr110mwOneSecond,
    kSht4xCommandActivateHtr110mwTenthSecond,
    kSht4xCommandActivateHtr20mwOneSecond,
    kSht4xCommandActivateHtr20mwTenthSecond
};

/*
 * All commands except RESET returns 6 bytes.
 */

typedef uint8_t Sht4xSerialNumber_t[kSht4xSerialLength];

class I2cSht4x : public TemperatureInterface, public RelativeHumidityInterface {
 public:

    uint8_t deviceAddress();

    void init();

    bool getSerialNumber(Sht4xSerialNumber_t *serial_number);

    void softReset();

    float getTemperature(TemperatureUnit_t unit);

    float getRelativeHumidity();

    timeslice getMeasurementInterval();

    void setMeasurementInterval(timeslice interval);

 private:
   timeslice measurement_time_{1000};  /* One second between measurements */
   std::chrono::time_point<std::chrono::steady_clock> last_read_ = std::chrono::time_point<std::chrono::steady_clock>::min();
   std::atomic<int> temperature_measurement_;
   std::atomic<int> relative_humidity_measurement_;
   Sht4xSerialNumber_t serial_number_ = {0, 0, 0, 0, 0, 0};

   void getMeasurement(Sht4xMeasurmentMode mode);

   bool measurementExpired();

};

#endif // LIB_DEVICES_I2C_SHT4X_I2C_H
