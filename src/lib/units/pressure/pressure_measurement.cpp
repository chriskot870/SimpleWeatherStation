#include "pressure_measurement.h"

using std::chrono::system_clock;

namespace qw_units {

PressureMeasurement::PressureMeasurement() {}

PressureMeasurement::PressureMeasurement(Pressure value, Pressure accuracy, system_clock time) :
  value_(value),
  accuracy_(accuracy),
  time_(time) {}

Pressure PressureMeasurement::value() {
  return value_;
}

Pressure PressureMeasurement::accuracy() {
  return accuracy_;
}

system_clock PressureMeasurement::time() {
  return time_;
}

}  // Namespace qw_units