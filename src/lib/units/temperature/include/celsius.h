 /*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_CELSIUS_H_
#define LIB_UNITS_TEMPERATURE_CELSIUS_H_

#include <math.h>
#include <compare>
#include <string>
#include <fmt/format.h>
#include <variant>

#include "temperature.h"
#include "fahrenheit.h"
#include "kelvin.h"

using std::string;
using std::strong_ordering;
using fmt::format;

namespace qw_units {

/*
 * Need to predeclare these classes
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

  strong_ordering operator<=>(const Celsius& other) const;

  Celsius& operator=(const Celsius& other);

  Celsius& operator+=(const Celsius& other);

  Celsius& operator-=(const Celsius& other);

  const Celsius operator+(const Celsius &other) const;

  const Celsius operator-(const Celsius &other) const;

  /*
   * Here we want all the conversion to and from base to the
   * measurable un
   */
  int CelsiusToBase(float temp);

  float BaseToCelsius(int base);
  /*
   * These constructors are for casting
   */
  // Celsius(const Fahrenheit& obj);

  //Celsius(const Kelvin& obj);

  /*
   * I thought I needed these for casting but found a site
   * that used the above.
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

  void setBaseValue(int base_value);

};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_