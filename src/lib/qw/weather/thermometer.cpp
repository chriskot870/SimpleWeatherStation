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

#include "qw/weather/include/thermometer.h"

#include <chrono>
#include <functional>
#include <expected>

#include "qw/units/temperature/include/temperature_measurement.h"

using std::expected;
using qw::units::TemperatureMeasurement;
using std::chrono::milliseconds;

namespace qw::weather {

Thermometer::Thermometer(
    std::function<expected<TemperatureMeasurement, int>()> getTemperature,
    std::function<milliseconds()> getInterval,
    std::function<void(milliseconds)> setInterval)
    : getTemperature_(getTemperature),
      getInterval_(getInterval),
      setInterval_(setInterval) {}

expected<TemperatureMeasurement, int> Thermometer::getData() {
  /*
  if (getTemperature_) {
    return getTemperature_();
  }
   */
  return getTemperature_();
}

milliseconds Thermometer::getInterval() {
  //
  // if (getInterval_) {
  //   return getInterval_();
  //}

  return getInterval_();
}

void Thermometer::setInterval(milliseconds interval) {
  //
  // if (setInterval_) {
  //   return setInterval_(interval);
  //}

  return setInterval_(interval);
}

}  // namespace qw::weather
