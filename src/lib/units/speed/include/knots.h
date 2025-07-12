/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_SPEED_KNOTS_H_
#define LIB_UNITS_SPEED_KNOTS_H_

#include "miles_per_hour.h"
#include "kilometers_per_hour.h"
#include "meters_per_second.h"
#include "speed.h"

namespace qw_units {

  /*
   * Need to predeclare these classes
   * I found that out the hard way
   */
  class MilesPerHour;
  class KilometersPerHour;
  class MetersPerSecond;

class Knots {

  friend MilesPerHour;
  friend KilometersPerHour;
  friend MetersPerSecond;

 public:
  Knots();

  Knots(float temp);

  Knots(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  bool operator==(const Knots& other) const;

  bool operator!=(const Knots& other) const;

  bool operator<(const Knots& other) const;

  bool operator>(const Knots& other) const;

  bool operator<=(const Knots& other) const;

  bool operator>=(const Knots& other) const;

  strong_ordering operator<=> (const Knots& other) const;

  Knots& operator=(const Knots& other);

  Knots& operator+=(const Knots& other);

  Knots& operator-=(const Knots& other);

  const Knots operator+(const Knots& other) const;

  const Knots operator-(const Knots& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator MilesPerHour() const;

  operator KilometersPerHour() const;

  operator MetersPerSecond() const;

 private:
  int64_t base_value_;

  string fmt_value_ = speed_default_format;
  
  Knots(int64_t base_value);

  int KnotsToBase(float temp);

  float BaseToKnots(int base);

  void setBaseValue(int base_value);

};

}  //qw_units Namespace

#endif  // LIB_UNITS_SPEED_KNOTS_H_
