#include "relative_humidity_measurement.h"

using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw_units {

RelativeHumidityMeasurement::RelativeHumidityMeasurement() {}

RelativeHumidityMeasurement::RelativeHumidityMeasurement(RelativeHumidity value, RelativeHumidity accuracy, time_point<system_clock> time) :
  value_(value),
  accuracy_(accuracy),
  time_(time) {}

RelativeHumidity RelativeHumidityMeasurement::value() {
  return value_;
}

RelativeHumidity RelativeHumidityMeasurement::accuracy() {
  return accuracy_;
}

time_point<system_clock> RelativeHumidityMeasurement::time() {
  return time_;
}

RelativeHumidity RelativeHumidityMeasurement::relativeHumidityValue() {
  return value_;
}

RelativeHumidity RelativeHumidityMeasurement::relativeHumidityAccuracy() {
  return accuracy_;
}
}  // Namespace qw_units