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