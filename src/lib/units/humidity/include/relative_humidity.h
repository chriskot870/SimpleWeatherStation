/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_HUMIDITY_RELATIVE_HUMIDITY_H_
#define LIB_UNITS_HUMIDITY_RELATIVE_HUMIDITY_H_

#include <math.h>
#include <compare>
#include <string>
#include <fmt/format.h>

using std::strong_ordering;
using std::string;
using fmt::format;

namespace qw_units {

constexpr int rh_base_conversion_factor = 100;
constexpr string relative_humidity_default_format = "{0:.1f}";

class RelativeHumidity {
 public:
  RelativeHumidity();

  RelativeHumidity(float rh);

  RelativeHumidity(float rh, string fmt_value);

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

  const RelativeHumidity operator+(const RelativeHumidity &other) const;

  const RelativeHumidity operator-(const RelativeHumidity &other) const;

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

 private:

   int base_value_;

   string fmt_value_ = relative_humidity_default_format;

   void setBaseValue(int base_value);
};

}  // Namespace qw_units

#endif  // LIB_UNITS_HUMIDITY_RELATIVE_HUMIDITY_H_