/*
 * Any measurement invlolves the reading the accuracy and the time of the reading
 */
#ifndef LIB_UNITS_PRESSURE_MEASUREMENT_H_
#define LIB_UNITS_PRESSURE_MEASUREMENT_H_

#include <chrono>
#include <variant>
#include "inches_mercury.h"
#include "millibar.h"
#include "pressure.h"

using std::variant;
using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw_units {

class PressureMeasurement {
 public:
  PressureMeasurement();

  PressureMeasurement(variant<Millibar, InchesMercury> value,
                      variant<Millibar, InchesMercury> accuracy,
                      time_point<system_clock> time);

  variant<Millibar, InchesMercury> value();

  variant<Millibar, InchesMercury> accuracy();

  time_point<system_clock> time();

  Millibar millibarValue();

  InchesMercury inchesMercuryValue();

  Millibar millibarAccuracy();

  InchesMercury inchesMercuryAccuracy();

 private:
  variant<Millibar, InchesMercury> value_;
  variant<Millibar, InchesMercury> accuracy_;
  time_point<system_clock> time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_PRESSURE_MEASUREMENT_H_