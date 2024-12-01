#include "relative_humidity_measurement.h"

using std::chrono::system_clock;

namespace qw_units {

RelativeHumidityMeasurement::RelativeHumidityMeasurement() {}

RelativeHumidityMeasurement::RelativeHumidityMeasurement(RelativeHumidity value, RelativeHumidity accuracy, system_clock time) :
  value_(value),
  accuracy_(accuracy),
  time_(time) {}

RelativeHumidity RelativeHumidityMeasurement::value() {
  return value_;
}

RelativeHumidity RelativeHumidityMeasurement::accuracy() {
  return accuracy_;
}

system_clock RelativeHumidityMeasurement::time() {
  return time_;
}

}  // Namespace qw_units