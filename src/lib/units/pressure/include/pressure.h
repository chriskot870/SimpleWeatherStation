/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_PRESSURE_H_
#define LIB_UNITS_PRESSURE_H_

#include <math.h>
#include <compare>

using std::strong_ordering;

namespace qw_units {
/*
 * Our pressure base is millicelsius so we want
 * to apply the pressur_base_conversion_factor
 * to millibar values.
 */
constexpr int pressure_base_conversion_factor = 1000;
constexpr float inHg_sea_level = 29.92;
constexpr float mb_sea_level = 1013.25;

class Pressure {
 public:
  Pressure();

  /*
   * This is basically used to create an object
   */
  Pressure(int base_value);

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

  int64_t base_value_;

};
}
#endif  // LIB_UNITS_PRESSURE_H_
