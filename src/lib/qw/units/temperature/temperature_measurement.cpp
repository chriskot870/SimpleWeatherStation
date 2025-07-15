/*
 * Copyright 2024,2025, Chris Kottaridis. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * NTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qw/units/temperature/include/temperature_measurement.h"

using std::string;
using std::get;
using std::holds_alternative;
using std::variant;
using std::chrono::system_clock;
using std::chrono::time_point;

//using qw::units::Celsius;
//using qw::units::Fahrenheit;

namespace qw::units {

TemperatureMeasurement::TemperatureMeasurement() {}

TemperatureMeasurement::TemperatureMeasurement(
    variant<Celsius, Fahrenheit, Kelvin> value,
    variant<Celsius, Fahrenheit, Kelvin> accuracy,
    time_point<system_clock> time)
    : value_(value), accuracy_(accuracy), time_(time) {}

variant<Celsius, Fahrenheit, Kelvin>
TemperatureMeasurement::value() {
  return value_;
}

variant<Celsius, Fahrenheit, Kelvin>
TemperatureMeasurement::accuracy() {
  return accuracy_;
}

time_point<system_clock> TemperatureMeasurement::time() {
  return time_;
}

Celsius TemperatureMeasurement::celsiusValue() {

  Celsius tempc;
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

Fahrenheit TemperatureMeasurement::fahrenheitValue() {

  Fahrenheit tempf;
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

Kelvin TemperatureMeasurement::kelvinValue() {

  Kelvin tempk;
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

Celsius TemperatureMeasurement::celsiusAccuracy() {

  Celsius tempc;
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

Fahrenheit TemperatureMeasurement::fahrenheitAccuracy() {

  Fahrenheit tempf;
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

Kelvin TemperatureMeasurement::kelvinAccuracy() {

  Kelvin tempk;
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