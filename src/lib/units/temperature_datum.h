/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_DATUM_H_
#define LIB_UNITS_TEMPERATURE_DATUM_H_

#include <cmath>
#include <string>

typedef enum {
  TEMPERATURE_UNIT_FAHRENHEIT,
  TEMPERATURE_UNIT_CELSIUS,
  TEMPERATURE_UNIT_KELVIN
} TemperatureUnit_t;

/*
 * These are helper static routines to convert temperatures
 */
class TemperatureDatum {
 public:
  /*
   * Static functions to perform conversions of value from unit change
   */
  static float celsiusToFahrenheit(float celsius);

  static float celsiusToKelvin(float celsius);

  static float fahrenheitToCelsius(float fahrenheit);

  static float fahrenheitToKelvin(float fahrenheit);

  static float kelvinToCelsius(float kelvin);

  static float kelvinToFahrenheit(float kelvin);

  /*
   * Constructor function
   */
  TemperatureDatum (float value,  TemperatureUnit_t unit);

  /*
   * Non-static functions to retreive private variables
   */

  /*
   * Return the value
   */
  float getValue();

  /*
   * Return the unit
   */
  TemperatureUnit_t getUnit();

 private:

  float value_;

  TemperatureUnit_t unit_;

  /*
   * This is milli-celsius and will be used for comparisons
   */
  int base_value_;
  
};

#endif  // LIB_UNITS_TEMPERATURE_DATUM_H_