/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_PRESSURE_MEASUREMENT_H_
#define LIB_UNITS_PRESSURE_MEASUREMENT_H_

#include <chrono>


#include "pressure_datum.h"

using std::chrono::time_point;
using std::chrono::system_clock;

class PressureMeasurement {
 public:
  PressureMeasurement (PressureDatum data, time_point<system_clock> clock_time);

  PressureDatum getData();

  time_point<system_clock> getTime();

 private:

  /*
   * The raw temparture value read from the device
   */
  PressureDatum data_;

  /*
   * A pointer to the routine to translate the device value to a pressure datum
   */
  PressureDatum (*translator_)(int32_t);

  /*
   * The time the measurement was made
   */
  time_point<system_clock> clock_time_;

};

#endif  // LIB_UNITS_PRESSURE_MEASUREMENT_H_