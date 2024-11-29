/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_H_
#define LIB_UNITS_TEMPERATURE_H_

#include <math.h>
#include <compare>

using std::strong_ordering;

namespace qw_units {

/*
 * Our temperature base is millicelsius so we want
 * to apply the temperature_base_conversion_factor
 * to Celsisu values.
 */
constexpr int temperature_base_conversion_factor = 1000;
constexpr float temperature_celsius_kelvin_offset = 273.15;

class Temperature {
 public:
  Temperature();

  /*
   * This is basically used to create an object
   */
  Temperature(int base_value);

   /*
    * I think I want the operator definitions here and all
    * comparison and arithmetic work will be done using
    * the base value.
    */

  bool operator==(const Temperature& other) const;

  bool operator!=(const Temperature& other) const;

  bool operator<(const Temperature& other) const;

  bool operator>(const Temperature& other) const;

  bool operator<=(const Temperature& other) const;

  bool operator>=(const Temperature& other) const;

  strong_ordering operator<=>(const Temperature& other) const;

  Temperature& operator=(const Temperature& other);

  Temperature& operator+=(const Temperature& other);

  Temperature& operator-=(const Temperature& other);

  /*
   * Here we want all the conversion to and from base to the
   * measurable un
   */
  int CelsiusToBase(float temp);

  float BaseToCelsius(int base);

  int FahrenheitToBase(float temp);

  float BaseToFahrenheit(int base);

  int KelvinToBase(float temp);

  float BaseToKelvin(int base);

  int64_t base_value_;

};

}

#endif  // LIB_UNITS_TEMPERATURE_H_
