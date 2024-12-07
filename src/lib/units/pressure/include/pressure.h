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

}  // Namespace qw_units

#endif  // LIB_UNITS_PRESSURE_H_
