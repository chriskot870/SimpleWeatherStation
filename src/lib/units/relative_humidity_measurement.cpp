/*
 * Copyright 2024 Chris Kottaridis
 */

#include "relative_humidity_measurement.h"

/*
 * Constructor function
 */
RelativeHumidityMeasurement::RelativeHumidityMeasurement(RelativeHumidityDatum data, time_point<system_clock> clock_time) : data_(data), clock_time_(clock_time) {}

/*
 * Return the data value
 */
RelativeHumidityDatum RelativeHumidityMeasurement::getData() {

  return data_;
}

/*
 * Return the clock time of the measurement
 */
time_point<system_clock> RelativeHumidityMeasurement::getTime() {

  return clock_time_;
}

