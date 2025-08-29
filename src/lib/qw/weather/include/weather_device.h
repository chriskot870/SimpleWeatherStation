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

#include <expected>
#include <chrono>
#include <functional>

#include "qw/units/include/unit_measurement.h"
#include "qw/units/temperature/include/temperature.h"

using std::expected;
using qw::units::UnitMeasurement;
using std::chrono::milliseconds;

namespace qw::weather {

template<typename Tdev>
class WeatherDevice {
 public:
  WeatherDevice(
    std::function<expected<UnitMeasurement<Tdev>, int>()> fetchData,
    std::function<milliseconds()> getInterval,
    std::function<void(milliseconds)> setInterval)
    : fetchData_(fetchData),
      getInterval_(getInterval),
      setInterval_(setInterval) {}

expected<UnitMeasurement<Tdev>, int> getData() {
  return fetchData_();
}

milliseconds getInterval() {
  return getInterval_();
}

void setInterval(milliseconds interval) {
  return setInterval_(interval);
}

 private:
  std::function<std::expected<qw::units::UnitMeasurement<Tdev>, int>()> fetchData_;
  std::function<std::chrono::milliseconds()> getInterval_;
  std::function<void(std::chrono::milliseconds)> setInterval_;
};

}  // namespace qw::weather
