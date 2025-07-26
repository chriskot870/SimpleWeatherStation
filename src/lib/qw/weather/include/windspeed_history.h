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

#ifndef SRC_LIB_QW_WEATHER_INCLUDE_WINDSPEED_HISTORY_H_
#define SRC_LIB_QW_WEATHER_INCLUDE_WINDSPEED_HISTORY_H_

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <cmath>
#include <deque>
#include <expected>

#include "qw/units/speed/include/miles_per_hour.h"
#include "qw/units/speed/include/speed.h"
#include "qw/units/speed/include/speed_measurement.h"

namespace qw::weather {

constexpr std::chrono::seconds kMaxListTimeSpan(60 *
                                                10);  // 10 minutes of samples
constexpr std::chrono::seconds kInterval10m(60 * 10);
constexpr std::chrono::seconds kInterval2m(60 * 2);

class WindspeedHistory {
 public:
  WindspeedHistory();

  void setMaximumTime(std::chrono::seconds time_span);

  std::chrono::seconds getMaximumTime();

  size_t size();

  size_t countOverPeriod(std::chrono::seconds time_span);

  void add(qw::units::SpeedMeasurement speed);

  std::expected<qw::units::Speed, int> average(std::chrono::seconds time_span);

  std::expected<qw::units::SpeedMeasurement, int> gust(
      std::chrono::seconds time_span);

 private:
  std::deque<qw::units::SpeedMeasurement> history_;

  std::chrono::seconds maximum_time_ = kMaxListTimeSpan;

  void prune();
};

}  // namespace qw::weather

#endif  // SRC_LIB_QW_WEATHER_INCLUDE_WINDSPEED_HISTORY_H_