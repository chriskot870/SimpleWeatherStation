#include "relative_humidity_measurement.h"

using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw_units {

/*
 * Constructor routines
 */
RelativeHumidityMeasurement::RelativeHumidityMeasurement() {}

RelativeHumidityMeasurement::RelativeHumidityMeasurement(RelativeHumidity value, RelativeHumidity accuracy, time_point<system_clock> time) :
  value_(value),
  accuracy_(accuracy),
  time_(time) {}

/*
 * Constructor routines
 */
RelativeHumidity RelativeHumidityMeasurement::value() {
  return value_;
}

RelativeHumidity RelativeHumidityMeasurement::accuracy() {
  return accuracy_;
}

time_point<system_clock> RelativeHumidityMeasurement::time() {
  return time_;
}

/*
 * These would make more sense if there was more than one
 * unit of relative humidity
 */
/*
 * This is a duplicate of value above for symmetry with other units
 */
RelativeHumidity RelativeHumidityMeasurement::relativeHumidityValue() {
  return value_;
}

/*
 * This is a duplicate of value accuracy for symmetry with other units
 */
RelativeHumidity RelativeHumidityMeasurement::relativeHumidityAccuracy() {
  return accuracy_;
}
}  // Namespace qw_units