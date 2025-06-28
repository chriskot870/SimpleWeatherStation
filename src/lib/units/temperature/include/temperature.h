/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_H_
#define LIB_UNITS_TEMPERATURE_H_

#include <string>

using std::string;

namespace qw_units {

/*
 * Our temperature base is millicelsius so we want
 * to apply the temperature_base_conversion_factor
 * to Celsisu values.
 */
constexpr int temperature_base_conversion_factor = 1000;
constexpr float temperature_celsius_kelvin_offset = 273.15;
constexpr string temperature_default_format = "{0:.2f}";

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_H_