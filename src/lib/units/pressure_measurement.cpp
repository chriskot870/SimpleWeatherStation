/*
 * Copyright 2024 Chris Kottaridis
 */

#include "pressure_measurement.h"

/*
 * Constructor
 */
PressureMeasurement::PressureMeasurement(PressureDatum data, time_point<system_clock> clock_time) : data_(data), clock_time_(clock_time) {}

/*
 * Returns the datum of the measurement
 */
PressureDatum PressureMeasurement::getData() {

  return data_;
}

/*
 * Returns system clock time the measurement was taken
 */
time_point<system_clock> PressureMeasurement::getTime() {

  return clock_time_;
}
