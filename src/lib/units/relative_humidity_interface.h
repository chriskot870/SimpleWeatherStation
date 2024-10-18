#ifndef LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H_
#define LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H_
/*
 * This contains the RelativeHumidityInterface
 */

#include "relative_humidity_measurement.h"

using std::expected;

class RelativeHumidityInterface {
 public:
  /*
   * The Relative Humidity is always in percent
   */

  virtual expected<RelativeHumidityMeasurement, int> getRelativeHumidityMeasurement() = 0;

  virtual ~RelativeHumidityInterface() {}
};

#endif  // LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H_
