/*
 * Copyright 2024 Chris Kottaridis
 */
#include <string.h>

#include "temperature_measurement.h"

TemperatureMeasurement::TemperatureMeasurement (TemperatureDatum data, time_point<system_clock> clock_time) : data_(data), clock_time_(clock_time) {}

TemperatureDatum TemperatureMeasurement::getDatum() {

  return data_;
}

time_point<system_clock> TemperatureMeasurement::getTime() {

  return clock_time_;
}

TemperatureMeasurement::~TemperatureMeasurement() { }
