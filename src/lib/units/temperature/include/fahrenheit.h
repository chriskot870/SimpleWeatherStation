/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_
#define LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_

#include <math.h>
#include <compare>
#include <string>
#include <fmt/format.h>
#include <variant>

#include "temperature.h"
#include "celsius.h"
#include "kelvin.h"

using std::string;
using std::strong_ordering;
using fmt::format;

namespace qw_units {
/*
 * Need to predeclare the friend classes
 * I found that out the hard way
 */

class Celsius;
class Kelvin;

class Fahrenheit {
  friend Celsius;
  friend Kelvin;

 public:
  Fahrenheit();

  Fahrenheit(float temp);

  Fahrenheit(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  /********************
   * Operator functions
   ********************/

  bool operator==(const Fahrenheit& other) const;

  bool operator!=(const Fahrenheit& other) const;

  bool operator<(const Fahrenheit& other) const;

  bool operator>(const Fahrenheit& other) const;

  bool operator<=(const Fahrenheit& other) const;

  bool operator>=(const Fahrenheit& other) const;

  strong_ordering operator<=>(const Fahrenheit& other) const;

  Fahrenheit& operator=(const Fahrenheit& other);

  Fahrenheit& operator+=(const Fahrenheit& other);

  Fahrenheit& operator-=(const Fahrenheit& other);

  const Fahrenheit operator+(const Fahrenheit &other) const;

  const Fahrenheit operator-(const Fahrenheit &other) const;

  operator Celsius() const;

  operator Kelvin() const;

 private:
  int64_t base_value_;

  string fmt_value_ = temperature_default_format;

  int FahrenheitToBase(float temp);

  float BaseToFahrenheit(int base);
  
  void setBaseValue(int base_value);
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_