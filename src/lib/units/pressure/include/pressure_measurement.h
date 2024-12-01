/*
 * Any measurement invlolves the reading the accuracy and the time of the reading
 */
#ifndef LIB_UNITS_PRESSURE_MEASUREMENT_H_
#define LIB_UNITS_PRESSURE_MEASUREMENT_H_

#include <chrono>
#include "pressure.h"

using std::chrono::system_clock;

namespace qw_units {

class PressureMeasurement {
 public:
  PressureMeasurement();

  PressureMeasurement(Pressure value, Pressure accuracy, system_clock time);

  Pressure value();

  Pressure accuracy();

  system_clock time();

 private:
  Pressure value_;
  Pressure accuracy_;
  system_clock time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_MEASUREMENT_H_