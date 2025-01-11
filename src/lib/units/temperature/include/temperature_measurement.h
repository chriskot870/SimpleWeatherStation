
/*
 * Any measurement invlolves the reading the accuracy and the time of the reading
 */
#ifndef LIB_UNITS_TEMPERATURE_MEASUREMENT_H_
#define LIB_UNITS_TEMPERATURE_MEASUREMENT_H_

#include <chrono>
#include <variant>

#include "celsius.h"
#include "fahrenheit.h"
#include "kelvin.h"
#include "temperature.h"

using std::variant;
using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw_units {

class TemperatureMeasurement {
 public:
  TemperatureMeasurement();

  TemperatureMeasurement(variant<Celsius, Fahrenheit, Kelvin> value,
                         variant<Celsius, Fahrenheit, Kelvin> accuracy,
                         time_point<system_clock> time);

  variant<Celsius, Fahrenheit, Kelvin> value();

  variant<Celsius, Fahrenheit, Kelvin> accuracy();

  time_point<system_clock> time();

  Celsius celsiusValue();

  Fahrenheit fahrenheitValue();

  Kelvin kelvinValue();

  Celsius celsiusAccuracy();

  Fahrenheit fahrenheitAccuracy();

  Kelvin kelvinAccuracy();

 private:
  variant<Celsius, Fahrenheit, Kelvin> value_;
  variant<Celsius, Fahrenheit, Kelvin> accuracy_;
  time_point<system_clock> time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_MEASUREMENT_H_