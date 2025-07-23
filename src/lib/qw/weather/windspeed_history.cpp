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
#include <algorithm>
#include "qw//weather/include/windspeed_history.h"

using std::expected;
using std::unexpected;
using std::sort;
using qw::units::Speed;
using qw::units::MilesPerHour;
using qw::units::SpeedMeasurement;
using qw::units::SpeedMeasurementClock;
using qw::units::SpeedMeasurementTimeStamp;

namespace qw::weather {

WindspeedHistory::WindspeedHistory() {}

void WindspeedHistory::setMaximumTime(std::chrono::seconds time_span) {
  
  maximum_time_ = time_span;

  /*
   * Since we changed the time span cleanup the list with the new maximum_time
   */
  prune();

  return;
}

std::chrono::seconds WindspeedHistory::getMaximumTime() {
  
  return maximum_time_;
}

size_t WindspeedHistory::size() {
  size_t count = 0;

  /*
   * Get rid of any old entries before returning a count
   */
  prune();

  count = history_.size();

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
  for (auto it = history_.rbegin(); it != history_.rend(); ++it) {
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
  if ((current_time - speed.time()) <= maximum_time_) {
    history_.push_back(speed);
  }

  /*
   * We need the deque to be ordered by time.
   * On the chance the measurements got added in different order
   * we want to sort by time whenever a new measurement is added.
   * This way we can be sure of the order of the measurements by time.
   * We use a lambda function to make the comparison.
   */
  sort(history_.begin(), history_.end(), [](SpeedMeasurement a, SpeedMeasurement b) {
    /*
     * We want the older times near the front.
     * So, if a.time() > b.time() return true.
     */
    if (a.time() > b.time()) {
      return true;
    }
    return false;
  } );

  return;
}

expected<Speed, int> WindspeedHistory::average(std::chrono::seconds time_span) {
  uint count = 0;
  Speed total(0);

  prune();

  /*
   * If history is empty return an error so we don't divide by 0
   */
  if (history_.empty() == true) {
    return unexpected(ENODATA);
  }

   /*
   * Iterate through the list backwards, which is from newest measurement.
   */
  SpeedMeasurementTimeStamp current_time = SpeedMeasurementClock::now();
  for (auto it = history_.rbegin(); it != history_.rend(); ++it) {
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
      total += speed;
    } else {
        break;
    }
  }

  Speed ave_mph = total / count;

  return ave_mph;
}

expected<SpeedMeasurement, int> WindspeedHistory::gust(std::chrono::seconds time_span) {

  prune();

  /*
   * Return error if there is no data
   */
  if (history_.empty() == true) {
    return unexpected(ENODATA);
  }

  SpeedMeasurementTimeStamp current_time = SpeedMeasurementClock::now();
  SpeedMeasurement max_measurement = history_.back();
  for (auto it = history_.rbegin(); it != history_.rend(); ++it) {
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

  SpeedMeasurementTimeStamp current_time = SpeedMeasurementClock::now();

  /*
   * We look at the fron of the history which is the oldest one.
   * If the time stamp is greater than the maximum time we pop it
   * off the history deque. We keep doing this till we run into an
   * element that is less than or equal to the maximum_time or there
   * are no elements left in the history.
   */
  while((history_.empty() != true) && ((current_time - history_.front().time()) > maximum_time_)) {
    history_.pop_front();
  }

  return;
}

}  // namespace qw::weather