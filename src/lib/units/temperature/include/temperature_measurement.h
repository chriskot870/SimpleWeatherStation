
/*
 * Any measurement invlolves the reading the accuracy and the time of the reading
 */
#ifndef LIB_UNITS_TEMPERATURE_MEASUREMENT_H_
#define LIB_UNITS_TEMPERATURE_MEASUREMENT_H_

#include <chrono>
#include "temperature.h"

using std::chrono::system_clock;

namespace qw_units {

class TemperatureMeasurement {
 public:
  TemperatureMeasurement();

  TemperatureMeasurement(qw_units::Temperature value, qw_units::Temperature accuracy, system_clock time);

  Temperature value();

  Temperature accuracy();

  system_clock time();

 private:
  Temperature value_;
  Temperature accuracy_;
  system_clock time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_MEASUREMENT_H_