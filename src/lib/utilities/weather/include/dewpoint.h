
 /*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UTILITIES_WEATHER_DEWPOINT_H_
#define LIB_UTILITIES_WEATHER_DEWPOINT_H_

#include <cmath>

#include "celsius.h"
#include "relative_humidity.h"

namespace qw_utilities {

using qw_units::Celsius;
using qw_units::RelativeHumidity;

constexpr float dew_point_b = 17.625;
const  float dew_point_c = 243.04;

Celsius dewPoint(Celsius tempc, RelativeHumidity rh);

}  // Namespace qw_utilties

#endif  // LIB_UTILITIES_WEATHER_DEWPOINT_H_