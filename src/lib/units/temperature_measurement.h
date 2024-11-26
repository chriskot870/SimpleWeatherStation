/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_MEASUREMENT_H_
#define LIB_UNITS_TEMPERATURE_MEASUREMENT_H_

#include <chrono>

#include "temperature_datum.h"

using std::chrono::time_point;
using std::chrono::system_clock;

class TemperatureMeasurement {
 public:
  TemperatureMeasurement (TemperatureDatum data, time_point<system_clock> clock_time);

  TemperatureDatum getDatum();

  time_point<system_clock> getTime();

  ~TemperatureMeasurement();

 private:
  /*
   * A poinmter to raw data from the device
   */
  TemperatureDatum data_;

  /*
   * The time the measurement was made
   */
  time_point<system_clock> clock_time_;

};

#endif  // LIB_UNITS_TEMPERATURE_MEASUREMENT_H_