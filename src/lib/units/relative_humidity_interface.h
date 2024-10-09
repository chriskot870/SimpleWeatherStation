#ifndef LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H
#define LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H
/*
 * This contains the RelativeHumidityInterface
 */

class RelativeHumidityInterface {
 public:
  /*
   * The Relative Humidity in percent
   */

  virtual float getRelativeHumidity() = 0;

  virtual ~RelativeHumidityInterface() {}
};

#endif  // LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H
