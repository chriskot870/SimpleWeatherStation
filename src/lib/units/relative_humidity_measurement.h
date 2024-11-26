/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_RELATIVE_HUMIDITY_MEASUREMENT_H_
#define LIB_UNITS_RELATIVE_HUMIDITY_MEASUREMENT_H_

#include <chrono>

using std::chrono::time_point;
using std::chrono::system_clock;

#include "relative_humidity_datum.h"

class RelativeHumidityMeasurement {
 public:
  /*
   * The constructor routine
   */
  RelativeHumidityMeasurement (RelativeHumidityDatum data, time_point<system_clock> clock_time);

  /*
   * Return the data of the measurement
   */
  RelativeHumidityDatum getDatum();

  /*
   * Return the time of the measurement
   */
  time_point<system_clock> getTime();

 private:
  /*
   * The actual temperature measurement
   */
  RelativeHumidityDatum data_;

  /*
   * The time the measurement was made
   */
  time_point<system_clock> clock_time_;

};

#endif  // LIB_UNITS_RELATIVE_HUMIDITY_MEASUREMENT_H_