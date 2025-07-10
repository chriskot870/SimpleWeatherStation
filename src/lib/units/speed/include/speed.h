/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_SPEED_H_
#define LIB_UNITS_SPEED_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>

using fmt::format;
using std::string;
using std::strong_ordering;

namespace qw_units {
/*
 * Our speed is based on .01 mile per hour
 */
constexpr int speed_base_conversion_factor = 100;
constexpr float kMilesPerKilometer = .621;  // 0.621371 I assume 3 digits is enough
constexpr float kKilometersPerMile = (1/kMilesPerKilometer);  // The inverse of above
constexpr float kMpsPerMph = 2.24;   // 2.23694 mph per mps
constexpr float kMphPerMps = (1/kMpsPerMph);  // The inverse of above
constexpr float kMphPerKnot = 1.15;  // 1.15078 I assume 3 digits is good enough
constexpr float kKnotPerMph = (1/kMphPerKnot);  // The inverse of above.
const string speed_default_format = "{0:.2f}";

}  // Namespace qw_units

#endif  // LIB_UNITS_SPEED_H_