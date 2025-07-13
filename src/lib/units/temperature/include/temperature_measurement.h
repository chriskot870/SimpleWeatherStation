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

/*
 * Any measurement invlolves the reading the accuracy and the time of the reading
 */
#ifndef LIB_UNITS_TEMPERATURE_MEASUREMENT_H_
#define LIB_UNITS_TEMPERATURE_MEASUREMENT_H_

#include <chrono>
#include <variant>

#include "celsius.h"
#include "fahrenheit.h"
#include "kelvin.h"
#include "temperature.h"

using std::variant;
using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw_units {

class TemperatureMeasurement {
 public:
  TemperatureMeasurement();

  TemperatureMeasurement(variant<Celsius, Fahrenheit, Kelvin> value,
                         variant<Celsius, Fahrenheit, Kelvin> accuracy,
                         time_point<system_clock> time);

  variant<Celsius, Fahrenheit, Kelvin> value();

  variant<Celsius, Fahrenheit, Kelvin> accuracy();

  time_point<system_clock> time();

  Celsius celsiusValue();

  Fahrenheit fahrenheitValue();

  Kelvin kelvinValue();

  Celsius celsiusAccuracy();

  Fahrenheit fahrenheitAccuracy();

  Kelvin kelvinAccuracy();

 private:
  variant<Celsius, Fahrenheit, Kelvin> value_;
  variant<Celsius, Fahrenheit, Kelvin> accuracy_;
  time_point<system_clock> time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_MEASUREMENT_H_