
/*
 * Any measurement invlolves the reading the accuracy and the time of the reading
 */
#ifndef LIB_UNITS_TEMPERATURE_MEASUREMENT_H_
#define LIB_UNITS_TEMPERATURE_MEASUREMENT_H_

#include <chrono>
#include <variant>

#include "temperature.h"
#include "celsius.h"
#include "fahrenheit.h"
#include "kelvin.h"

using std::chrono::system_clock;
using std::chrono::time_point;
using std::variant;

namespace qw_units {

class TemperatureMeasurement {
 public:
  TemperatureMeasurement();

  TemperatureMeasurement(variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> value,
                        variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> accuracy,
                        time_point<system_clock> time);

  variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> value();

  variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> accuracy();

  time_point<system_clock> time();

  qw_units::Celsius celsiusValue();

  qw_units::Fahrenheit fahrenheitValue();

  qw_units::Kelvin kelvinValue();

  qw_units::Celsius celsiusAccuracy();

  qw_units::Fahrenheit fahrenheitAccuracy();

  qw_units::Kelvin kelvinAccuracy();

 private:
  variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> value_;
  variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> accuracy_;
  time_point<system_clock> time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_MEASUREMENT_H_