/*
 * Any measurement invlolves the reading the accuracy and the time of the reading
 */
#ifndef LIB_UNITS_PRESSURE_MEASUREMENT_H_
#define LIB_UNITS_PRESSURE_MEASUREMENT_H_

#include <chrono>
#include <variant>
#include "pressure.h"
#include "inches_mercury.h"
#include "millibar.h"

using std::chrono::system_clock;
using std::chrono::time_point;
using std::variant;

namespace qw_units {

class PressureMeasurement {
 public:
  PressureMeasurement();

  PressureMeasurement(variant<qw_units::Millibar, qw_units::InchesMercury> value,
                      variant<qw_units::Millibar, qw_units::InchesMercury> accuracy,
                      time_point<system_clock> time);

  variant<qw_units::Millibar, qw_units::InchesMercury> value();

  variant<qw_units::Millibar, qw_units::InchesMercury> accuracy();

  time_point<system_clock> time();

  qw_units::Millibar millibarValue();

  qw_units::InchesMercury inchesMercuryValue();

  qw_units::Millibar millibarAccuracy();

  qw_units::InchesMercury inchesMercuryAccuracy();


 private:
  variant<qw_units::Millibar, qw_units::InchesMercury> value_;
  variant<qw_units::Millibar, qw_units::InchesMercury> accuracy_;
  time_point<system_clock> time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_MEASUREMENT_H_