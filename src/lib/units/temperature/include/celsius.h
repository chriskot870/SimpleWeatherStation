/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_CELSIUS_H_
#define LIB_UNITS_TEMPERATURE_CELSIUS_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>
#include <variant>

#include "fahrenheit.h"
#include "kelvin.h"
#include "temperature.h"

using fmt::format;
using std::string;
using std::strong_ordering;

namespace qw_units {

/*
 * Need to predeclare the friend classes
 * I found that out the hard way
 */
class Fahrenheit;
class Kelvin;

class Celsius {

  friend Fahrenheit;
  friend Kelvin;

 public:
  Celsius();

  Celsius(float temp);

  Celsius(float temp, string fmt_value);

  bool operator==(const Celsius& other) const;

  bool operator!=(const Celsius& other) const;

  bool operator<(const Celsius& other) const;

  bool operator>(const Celsius& other) const;

  bool operator<=(const Celsius& other) const;

  bool operator>=(const Celsius& other) const;

  strong_ordering operator<=> (const Celsius& other) const;

  Celsius& operator=(const Celsius& other);

  Celsius& operator+=(const Celsius& other);

  Celsius& operator-=(const Celsius& other);

  const Celsius operator+(const Celsius& other) const;

  const Celsius operator-(const Celsius& other) const;

  /*
   * These constructors are for casting
   */
  operator Fahrenheit() const;

  operator Kelvin() const;

  //Celsius& operator=(const Celsius& other);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

 private:
  int64_t base_value_;

  string fmt_value_ = temperature_default_format;

  /*
   * Here we want all the conversion to and from base to the
   */
  int CelsiusToBase(float temp);

  float BaseToCelsius(int base);

  /*
   * USed for conversion for implicit casting
   */
  void setBaseValue(int base_value);
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_CELSIUS_H_