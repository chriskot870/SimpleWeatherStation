/*
 * This contains the abstract class for temperature
 */
#ifndef LIB_UNITS_TEMPERATURE_INTERFACE_H_
#define LIB_UNITS_TEMPERATURE_INTERFACE_H_

#include "temperature_measurement.h"

class TemperatureInterface {
 public:
  /*
   * This routine needs to be implemented by the class derived from this interface class
   */
  virtual std::expected<TemperatureMeasurement, int> getTemperatureMeasurement(TemperatureUnit_t unit) = 0;

  virtual ~TemperatureInterface() {}
};

#endif  // LIB_UNITS_TEMPERATURE_INTERFACE_H_
