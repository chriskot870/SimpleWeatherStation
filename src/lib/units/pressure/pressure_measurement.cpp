#include "pressure_measurement.h"

using std::get;
using std::holds_alternative;
using std::variant;
using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw_units {

PressureMeasurement::PressureMeasurement() {}

PressureMeasurement::PressureMeasurement(
    variant<Millibar, InchesMercury> value,
    variant<Millibar, InchesMercury> accuracy, time_point<system_clock> time)
    : value_(value), accuracy_(accuracy), time_(time) {}

variant<Millibar, InchesMercury> PressureMeasurement::value() {
  return value_;
}

variant<Millibar, InchesMercury> PressureMeasurement::accuracy() {
  return accuracy_;
}

time_point<system_clock> PressureMeasurement::time() {
  return time_;
}

Millibar PressureMeasurement::millibarValue() {

  Millibar mbars;

  if (holds_alternative<InchesMercury>(value_)) {
    mbars = get<InchesMercury>(value_);
  }
  if (holds_alternative<Millibar>(value_)) {
    mbars = get<Millibar>(value_);
  }

  return mbars;
}

InchesMercury PressureMeasurement::inchesMercuryValue() {

  InchesMercury inches;

  if (holds_alternative<InchesMercury>(value_)) {
    inches = get<InchesMercury>(value_);
  }
  if (holds_alternative<Millibar>(value_)) {
    inches = get<Millibar>(value_);
  }

  return inches;
}

Millibar PressureMeasurement::millibarAccuracy() {

  Millibar mbars;

  if (holds_alternative<InchesMercury>(accuracy_)) {
    mbars = get<InchesMercury>(accuracy_);
  }
  if (holds_alternative<Millibar>(accuracy_)) {
    mbars = get<Millibar>(accuracy_);
  }

  return mbars;
}

InchesMercury PressureMeasurement::inchesMercuryAccuracy() {

  InchesMercury inches;

  if (holds_alternative<InchesMercury>(accuracy_)) {
    inches = get<InchesMercury>(accuracy_);
  }
  if (holds_alternative<Millibar>(accuracy_)) {
    inches = get<Millibar>(accuracy_);
  }

  return inches;
}

}  // Namespace qw_units