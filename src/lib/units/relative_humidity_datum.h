/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_RELATIVE_HUMIDITY_DATUM_H_
#define LIB_UNITS_RELATIVE_HUMIDITY_DATUM_H_

#include <string>

/*
 * We really only have 1 unit which is %
 */
typedef enum {
  RELATIVE_HUMIDITY_UNIT_PERCENT
} RelativeHumidityUnit_t;

/*
 * These are helper static routines to convert temperatures
 */
class RelativeHumidityDatum {
 public:
  /*
   * Constructor function
   */
  RelativeHumidityDatum (float value,  RelativeHumidityUnit_t unit);

  /*
   * Non-static functions to retreive private variables
   */

  /*
   * Return the value
   */
  float getValue();

  /*
   * Return the unit
   */
  RelativeHumidityUnit_t getUnit();

 private:

  float value_;

  RelativeHumidityUnit_t unit_;
};

#endif  // LIB_UNITS_RELATIVE_HUMIDITY_DATUM_H_