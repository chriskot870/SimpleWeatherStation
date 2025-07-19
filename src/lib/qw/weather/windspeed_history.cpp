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
#include "qw//weather/include/windspeed_history.h"

using std::expected;
using std::unexpected;
using qw::units::Speed;
using qw::units::MilesPerHour;
using qw::units::SpeedMeasurement;
using qw::units::SpeedMeasurementClock;
using qw::units::SpeedMeasurementTimeStamp;

namespace qw::weather {

WindspeedHistory::WindspeedHistory() {}

void WindspeedHistory::setMaximumTime(std::chrono::seconds time_span) {
  
  maximum_time = time_span;

  /*
   * Since we changed the time span cleanup the list with the new maximum_time
   */
  prune();

  return;
}

std::chrono::seconds WindspeedHistory::getMaximumTime() {
  
  return maximum_time;
}

size_t WindspeedHistory::size() {
  size_t count = 0;

  /*
   * Get rid of any old entries before returning a count
   */
  prune();

  count = history.size();

  return count;
}

size_t WindspeedHistory::countOverPeriod(std::chrono::seconds time_span) {
  size_t count = 0;
  SpeedMeasurementTimeStamp current_time = SpeedMeasurementClock::now();
  /*
   * Get rid of any old entries before returning a count
   */
  prune();

  /*
   * Iterate through the list backwards, which is from newest measurement.
   */
  for (auto it = history.rbegin(); it != history.rend(); ++it) {
    if ((current_time - (*it).time()) <= time_span) {
      count++;
    }
  }

  return count;
}

void WindspeedHistory::add(SpeedMeasurement speed) {

  /*
   * We take every opportunity to keep the list small
   * so clean up measurements that are out of range before
   * adding any new items.
   */
  prune();

  /*
   * Make sure the speed is within the maximum time
   */
  SpeedMeasurementTimeStamp current_time = SpeedMeasurementClock::now();
  if ((current_time - speed.time()) <= maximum_time) {
    history.push_back(speed);
  }

  return;
}

expected<Speed, int> WindspeedHistory::average(std::chrono::seconds time_span) {
  size_t count = 0;
  int64_t base_total = 0;

  prune();

  /*
   * If history is empty return an error so we don't divide by 0
   */
  if (history.empty() == true) {
    return unexpected(ENODATA);
  }

   /*
   * Iterate through the list backwards, which is from newest measurement.
   */
  SpeedMeasurementTimeStamp current_time = SpeedMeasurementClock::now();
  for (auto it = history.rbegin(); it != history.rend(); ++it) {
    /*
     * If it is within the time stamp add the value
     */
    if ((current_time - (*it).time()) <= time_span) {
      count++;
      /*
       * Now get the base_value of the measurment value.
       * add it to the total. 
       * Then create a new mph for it.
       * SpeedHistory has to be a friend of MilesPerHour for
       * this to work.
       */
      Speed speed = (*it).value();
      base_total += speed.getBaseValue();
    } else {
        break;
    }
  }

  int64_t ave_base = base_total / count;
  Speed ave_mph(ave_base);

  return ave_mph;
}

expected<SpeedMeasurement, int> WindspeedHistory::gust(std::chrono::seconds time_span) {

  prune();

  /*
   * Return error if there is no data
   */
  if (history.empty() == true) {
    return unexpected(ENODATA);
  }

  SpeedMeasurementTimeStamp current_time = SpeedMeasurementClock::now();
  SpeedMeasurement max_measurement = history.back();
  for (auto it = history.rbegin(); it != history.rend(); ++it) {
    /*
     * If we go past the time_span then exit loop
     */
    if ((current_time - (*it).time()) > time_span) {
      break;
    }
    /*
     * If this value is greater then the maximum, make it the maximum
     */
    if ((*it).value() > max_measurement.value()) {
        max_measurement = (*it);
    }
  }

  return max_measurement;
}

void WindspeedHistory::prune() {

  SpeedMeasurement measurement;
  MilesPerHour mph;
  SpeedMeasurementTimeStamp current_time = SpeedMeasurementClock::now();

  while((history.size() > 0) && ((current_time - history.front().time()) > maximum_time)) {
    history.pop_front();
  }

  return;
}

}  // namespace qw::weather