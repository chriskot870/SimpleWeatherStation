/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_PRESSURE_H_
#define LIB_UNITS_PRESSURE_H_

#include <math.h>
#include <compare>
#include <string>
#include <fmt/format.h>

using std::strong_ordering;
using std::string;
using fmt::format;

namespace qw_units {
/*
 * Our pressure base is millicelsius so we want
 * to apply the pressur_base_conversion_factor
 * to millibar values.
 */
constexpr int pressure_base_conversion_factor = 1000;
constexpr float inHg_sea_level = 29.92;
constexpr float mb_sea_level = 1013.25;
constexpr string pressure_default_format = "{0:.2f}";

class Millibar;
class InchesMercury;

class Pressure {

  friend Millibar;
  friend InchesMercury;

 public:
  Pressure();

   /*
    * I think I want the operator definitions here and all
    * comparison and arithmetic work will be done using
    * the base value.
    */
  auto operator<=>(const Pressure& other) const;

  auto operator==(const Pressure& other) const;

  auto operator!=(const Pressure& other) const;

  auto operator<(const Pressure& other) const;

  auto operator>(const Pressure& other) const;

  auto operator<=(const Pressure& other) const;

  auto operator>=(const Pressure& other) const;
 
  /*
   * Here we want all the conversion to and from base to the
   * measurable un
   */
  int InchesMercuryToBase(float temp);

  float BaseToInchesMercury(int base);

  int MillibarToBase(float temp);

  float BaseToMillibar(int base);

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

 private:
  int64_t base_value_;

  string fmt_value_ = pressure_default_format;

  void setBaseValue(int base_value);
};

}  // Namespace qw_units

#endif  // LIB_UNITS_PRESSURE_H_
