 /*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_PRESSURE_MILLIBAR_H_
#define LIB_UNITS_PRESSURE_MILLIBAR_H_

#include <math.h>
#include <compare>
#include <string>
#include <fmt/format.h>

#include "pressure.h"
#include "inches_mercury.h"

using std::strong_ordering;
using std::string;
using fmt::format;

namespace qw_units {
/*
 * Need to predeclare these classes
 * I found that out the hard way
 */
class InchesMercury;

class Millibar {

  friend InchesMercury;

 public:
  Millibar();

  Millibar(float temp);

  Millibar(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  bool operator==(const Millibar& other) const;

  bool operator!=(const Millibar& other) const;

  bool operator<(const Millibar& other) const;

  bool operator>(const Millibar& other) const;

  bool operator<=(const Millibar& other) const;

  bool operator>=(const Millibar& other) const;

  strong_ordering operator<=>(const Millibar& other) const;

  Millibar& operator=(const Millibar& other);

  Millibar& operator+=(const Millibar& other);

  Millibar& operator-=(const Millibar& other);

  const Millibar operator+(const Millibar &other) const;

  const Millibar operator-(const Millibar &other) const;

  /*
   * Supports implicit casting
   */
  operator InchesMercury() const;

  private:
  int64_t base_value_;

  string fmt_value_ = pressure_default_format;

  int MillibarToBase(float temp);

  float BaseToMillibar(int base);
  
  void setBaseValue(int base_value);

};

}  // Namespace qw_units

#endif  // LIB_UNITS_PRESSURE_MILLIBAR_H_