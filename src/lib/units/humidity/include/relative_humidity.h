/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_HUMIDITY_RELATIVE_HUMIDITY_H_
#define LIB_UNITS_HUMIDITY_RELATIVE_HUMIDITY_H_

#include <math.h>
#include <compare>

using std::strong_ordering;

namespace qw_units {

constexpr int rh_base_conversion_factor = 100;

class RelativeHumidity {
 public:
  RelativeHumidity();

  RelativeHumidity(float rh);

  RelativeHumidity(int rh);

  bool operator==(const RelativeHumidity& other) const;

  bool operator!=(const RelativeHumidity& other) const;

  bool operator<(const RelativeHumidity& other) const;

  bool operator>(const RelativeHumidity& other) const;

  bool operator<=(const RelativeHumidity& other) const;

  bool operator>=(const RelativeHumidity& other) const;

  strong_ordering operator<=>(const RelativeHumidity& other) const;

  RelativeHumidity& operator=(const RelativeHumidity& other);

  RelativeHumidity& operator+=(const RelativeHumidity& other);

  RelativeHumidity& operator-=(const RelativeHumidity& other);

  float getValue();

 private:

   int base_value_;
};

}

#endif  // LIB_UNITS_PRESSURE_INCHES_MERCURY_H_