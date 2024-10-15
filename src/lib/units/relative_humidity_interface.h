#ifndef LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H_
#define LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H_
/*
 * This contains the RelativeHumidityInterface
 */

using std::expected;

class RelativeHumidityInterface {
 public:
  /*
   * The Relative Humidity in percent
   */

  virtual expected<float, int> getRelativeHumidity() = 0;

  virtual ~RelativeHumidityInterface() {}
};

#endif  // LIB_UNITS_RELATIVE_HUMIDITY_INTERFACE_H_
