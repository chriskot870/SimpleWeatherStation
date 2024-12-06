#include "pressure_measurement.h"

using std::chrono::system_clock;
using std::chrono::time_point;
using std::variant;
using std::holds_alternative;
using std::get;

namespace qw_units {

PressureMeasurement::PressureMeasurement() {}

PressureMeasurement::PressureMeasurement(variant<qw_units::Millibar, qw_units::InchesMercury> value,
                        variant<qw_units::Millibar, qw_units::InchesMercury> accuracy,
                        time_point<system_clock> time) : value_(value), accuracy_(accuracy), time_(time) {}

variant<qw_units::Millibar, qw_units::InchesMercury> PressureMeasurement::value() {
  return value_;
}

variant<qw_units::Millibar, qw_units::InchesMercury> PressureMeasurement::accuracy() {
  return accuracy_;
}

time_point<system_clock> PressureMeasurement::time() {
  return time_;
}

qw_units::Millibar PressureMeasurement::millibarValue() {

  qw_units::Millibar mbars;

  if (holds_alternative<InchesMercury>(value_)) {
    mbars = get<InchesMercury>(value_);
  }
  if (holds_alternative<Millibar>(value_)) {
    mbars = get<Millibar>(value_);
  }

  return mbars;
}

qw_units::InchesMercury PressureMeasurement::inchesMercuryValue() {

  qw_units::InchesMercury inches;

  if (holds_alternative<InchesMercury>(value_)) {
    inches = get<InchesMercury>(value_);
  }
  if (holds_alternative<Millibar>(value_)) {
    inches = get<Millibar>(value_);
  }

  return inches;
}

qw_units::Millibar PressureMeasurement::millibarAccuracy() {

  qw_units::Millibar mbars;

  if (holds_alternative<InchesMercury>(accuracy_)) {
    mbars = get<InchesMercury>(accuracy_);
  }
  if (holds_alternative<Millibar>(accuracy_)) {
    mbars = get<Millibar>(accuracy_);
  }

  return mbars;
}

qw_units::InchesMercury PressureMeasurement::inchesMercuryAccuracy() {

  qw_units::InchesMercury inches;

  if (holds_alternative<InchesMercury>(accuracy_)) {
    inches = get<InchesMercury>(accuracy_);
  }
  if (holds_alternative<Millibar>(accuracy_)) {
    inches = get<Millibar>(accuracy_);
  }

  return inches;
}

}  // Namespace qw_units