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
#ifndef SRC_LIB_QW_UNITS_TEMPERATURE_INCLUDE_TEMPERATURE_MEASUREMENT_H_
#define SRC_LIB_QW_UNITS_TEMPERATURE_INCLUDE_TEMPERATURE_MEASUREMENT_H_

#include <chrono>

#include "qw/units/temperature/include/temperature.h"

namespace qw::units {

class TemperatureMeasurement {
 public:
  TemperatureMeasurement();

  TemperatureMeasurement(
      Temperature value, Temperature accuracy,
      std::chrono::time_point<std::chrono::system_clock> time);

  Temperature value();

  Temperature accuracy();

  std::chrono::time_point<std::chrono::system_clock> time();

 private:
  Temperature value_;
  Temperature accuracy_;
  std::chrono::time_point<std::chrono::system_clock> time_;
};

}  // namespace qw::units

#endif  // SRC_LIB_QW_UNITS_TEMPERATURE_INCLUDE_TEMPERATURE_MEASUREMENT_H_
