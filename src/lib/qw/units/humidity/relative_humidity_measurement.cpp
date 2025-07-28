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

#include "qw/units/humidity/include/relative_humidity_measurement.h"

using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw::units {

/*
 * Constructor routines
 */
RelativeHumidityMeasurement::RelativeHumidityMeasurement() {}

RelativeHumidityMeasurement::RelativeHumidityMeasurement(
    RelativeHumidity value, RelativeHumidity accuracy,
    time_point<std::chrono::system_clock> time)
    : value_(value), accuracy_(accuracy), time_(time) {}

/*
 * Constructor routines
 */
RelativeHumidity RelativeHumidityMeasurement::value() {
  return value_;
}

RelativeHumidity RelativeHumidityMeasurement::accuracy() {
  return accuracy_;
}

time_point<system_clock> RelativeHumidityMeasurement::time() {
  return time_;
}

/*
 * These would make more sense if there was more than one
 * unit of relative humidity
 */
/*
 * This is a duplicate of value above for symmetry with other units
 */
RelativeHumidity RelativeHumidityMeasurement::relativeHumidityValue() {
  return value_;
}

/*
 * This is a duplicate of value accuracy for symmetry with other units
 */
RelativeHumidity RelativeHumidityMeasurement::relativeHumidityAccuracy() {
  return accuracy_;
}
}  // namespace qw::units
