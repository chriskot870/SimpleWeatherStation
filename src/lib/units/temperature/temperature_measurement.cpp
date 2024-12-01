
#include "temperature_measurement.h"

using std::chrono::system_clock;

namespace qw_units {

TemperatureMeasurement::TemperatureMeasurement() {}

TemperatureMeasurement::TemperatureMeasurement(Temperature value, Temperature accuracy, system_clock time) :
  value_(value),
  accuracy_(accuracy),
  time_(time) {}

Temperature TemperatureMeasurement::value() {
  return value_;
}

Temperature TemperatureMeasurement::accuracy() {
  return accuracy_;
}

system_clock TemperatureMeasurement::time() {
  return time_;
}

}  // Namespace qw_units