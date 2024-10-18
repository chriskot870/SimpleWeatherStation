#ifndef LIB_UNITS_PRESSURE_INTERFACE_H_
#define LIB_UNITS_PRESSURE_INTERFACE_H_

#include <expected>
/*
 * This contains the BarometricPressureInterface
 */

#include "pressure_measurement.h"

class BarometricPressureInterface {
 public:
  /*
   * The Barometric Pressure
   */

  virtual std::expected<PressureMeasurement, int> getPressureMeasurement(PressureUnit_t unit) = 0;

  virtual ~BarometricPressureInterface() {}
};

#endif  // LIB_UNITS_BAROMETRIC_PRESSURE_INTERFACE_H_
