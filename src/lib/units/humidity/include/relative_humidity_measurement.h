/*
 * Any measurement invlolves the reading the accuracy and the time of the reading
 */
#ifndef LIB_UNITS_RELATIVE_HUMIDITY_MEASUREMENT_H_
#define LIB_UNITS_RELATIVE_HUMIDITY_MEASUREMENT_H_

#include <chrono>
#include "relative_humidity.h"

using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw_units {

class RelativeHumidityMeasurement {
 public:
  RelativeHumidityMeasurement();

  RelativeHumidityMeasurement(RelativeHumidity value, RelativeHumidity accuracy, time_point<system_clock> time);

  RelativeHumidity value();

  RelativeHumidity accuracy();

  time_point<system_clock> time();

  RelativeHumidity relativeHumidityValue();

  RelativeHumidity relativeHumidityAccuracy();

 private:
  RelativeHumidity value_;
  RelativeHumidity accuracy_;
  time_point<system_clock> time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_RELATIVE_HUMIDITY_MEASUREMENT_H_