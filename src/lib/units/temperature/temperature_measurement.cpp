
#include "temperature_measurement.h"

using std::chrono::system_clock;
using std::variant;
using std::get;
using std::holds_alternative;

namespace qw_units {

TemperatureMeasurement::TemperatureMeasurement() {}

TemperatureMeasurement::TemperatureMeasurement(variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> value,
                        variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> accuracy,
                        time_point<system_clock> time) : value_(value), accuracy_(accuracy), time_(time) {}

variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> TemperatureMeasurement::value() {
  return value_;
}

variant<qw_units::Celsius, qw_units::Fahrenheit, qw_units::Kelvin> TemperatureMeasurement::accuracy() {
  return accuracy_;
}

time_point<system_clock> TemperatureMeasurement::time() {
  return time_;
}

qw_units::Celsius TemperatureMeasurement::celsiusValue() {

  qw_units::Celsius tempc;
  if (holds_alternative<Celsius>(value_)) {
    tempc = get<Celsius>(value_);
  }
  if (holds_alternative<Fahrenheit>(value_)) {
    tempc = get<Fahrenheit>(value_);
  }
  if (holds_alternative<Kelvin>(value_)) {
    tempc = get<Kelvin>(value_);
  }

  return tempc;
}

qw_units::Fahrenheit TemperatureMeasurement::fahrenheitValue() {

  qw_units::Fahrenheit tempf;
  if (holds_alternative<Celsius>(value_)) {
    tempf = get<Celsius>(value_);
  }
  if (holds_alternative<Fahrenheit>(value_)) {
    tempf = get<Fahrenheit>(value_);
  }
  if (holds_alternative<Kelvin>(value_)) {
    tempf = get<Kelvin>(value_);
  }

  return tempf;
}

qw_units::Kelvin TemperatureMeasurement::kelvinValue() {

  qw_units::Kelvin tempk;
  if (holds_alternative<Celsius>(value_)) {
    tempk = get<Celsius>(value_);
  }
  if (holds_alternative<Fahrenheit>(value_)) {
    tempk = get<Fahrenheit>(value_);
  }
  if (holds_alternative<Kelvin>(value_)) {
    tempk = get<Kelvin>(value_);
  }

  return tempk;
}

qw_units::Celsius TemperatureMeasurement::celsiusAccuracy() {

  qw_units::Celsius tempc;
  if (holds_alternative<Celsius>(accuracy_)) {
    tempc = get<Celsius>(accuracy_);
  }
  if (holds_alternative<Fahrenheit>(accuracy_)) {
    tempc = get<Fahrenheit>(accuracy_);
  }
  if (holds_alternative<Kelvin>(accuracy_)) {
    tempc = get<Kelvin>(accuracy_);
  }

  return tempc;
}

qw_units::Fahrenheit TemperatureMeasurement::fahrenheitAccuracy() {

  qw_units::Fahrenheit tempf;
  if (holds_alternative<Celsius>(accuracy_)) {
    tempf = get<Celsius>(accuracy_);
  }
  if (holds_alternative<Fahrenheit>(accuracy_)) {
    tempf = get<Fahrenheit>(accuracy_);
  }
  if (holds_alternative<Kelvin>(accuracy_)) {
    tempf = get<Kelvin>(accuracy_);
  }

  return tempf;
}

qw_units::Kelvin TemperatureMeasurement::kelvinAccuracy() {

  qw_units::Kelvin tempk;
  if (holds_alternative<Celsius>(accuracy_)) {
    tempk = get<Celsius>(accuracy_);
  }
  if (holds_alternative<Fahrenheit>(accuracy_)) {
    tempk = get<Fahrenheit>(accuracy_);
  }
  if (holds_alternative<Kelvin>(accuracy_)) {
    tempk = get<Kelvin>(accuracy_);
  }

  return tempk;
}


}  // Namespace qw_units