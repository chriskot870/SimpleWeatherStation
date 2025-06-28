/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_KELVIN_H_
#define LIB_UNITS_TEMPERATURE_KELVIN_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>
#include <variant>

#include "celsius.h"
#include "fahrenheit.h"
#include "temperature.h"

using fmt::format;
using std::string;
using std::strong_ordering;

namespace qw_units {

/*
 * Need to predeclare the friend classes
 * I found this out the hard way
 */
class Celsius;
class Fahrenheit;

class Kelvin {
  friend Celsius;
  friend Fahrenheit;

 public:
  Kelvin();

  Kelvin(float temp);

  Kelvin(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  bool operator==(const Kelvin& other) const;

  bool operator!=(const Kelvin& other) const;

  bool operator<(const Kelvin& other) const;

  bool operator>(const Kelvin& other) const;

  bool operator<=(const Kelvin& other) const;

  bool operator>=(const Kelvin& other) const;

  strong_ordering operator<=> (const Kelvin& other) const;

  Kelvin& operator=(const Kelvin& other);

  Kelvin& operator+=(const Kelvin& other);

  Kelvin& operator-=(const Kelvin& other);

  const Kelvin operator+(const Kelvin& other) const;

  const Kelvin operator-(const Kelvin& other) const;

  operator Celsius() const;

  operator Fahrenheit() const;

 private:
  int64_t base_value_;

  string fmt_value_ = temperature_default_format;

  int KelvinToBase(float temp);

  float BaseToKelvin(int base);

  void setBaseValue(int base_value);
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_KELVIN_H_