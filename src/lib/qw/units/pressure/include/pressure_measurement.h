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
#ifndef LIB_UNITS_PRESSURE_MEASUREMENT_H_
#define LIB_UNITS_PRESSURE_MEASUREMENT_H_

#include <chrono>
#include <variant>
#include "qw/units/pressure/include/inches_mercury.h"
#include "qw/units/pressure/include/millibar.h"
#include "qw/units/pressure/include/pressure.h"

namespace qw::units {

class PressureMeasurement {
 public:
  PressureMeasurement();

  PressureMeasurement(std::variant<Millibar, InchesMercury> value,
                      std::variant<Millibar, InchesMercury> accuracy,
                      std::chrono::time_point<std::chrono::system_clock> time);

  std::variant<Millibar, InchesMercury> value();

  std::variant<Millibar, InchesMercury> accuracy();

  std::chrono::time_point<std::chrono::system_clock> time();

  Millibar millibarValue();

  InchesMercury inchesMercuryValue();

  Millibar millibarAccuracy();

  InchesMercury inchesMercuryAccuracy();

 private:
  std::variant<Millibar, InchesMercury> value_;
  std::variant<Millibar, InchesMercury> accuracy_;
  std::chrono::time_point<std::chrono::system_clock> time_;
};

}  // Namespace qw_units

#endif  // LIB_UNITS_PRESSURE_MEASUREMENT_H_