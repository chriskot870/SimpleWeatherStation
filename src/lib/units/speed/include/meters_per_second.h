/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_SPEED_METERS_PER_SECOND_H_
#define LIB_UNITS_SPEED_METERS_PER_SECOND_H_

#include "speed.h"
#include "miles_per_hour.h"
#include "kilometers_per_hour.h"
#include "knots.h"

namespace qw_units {

   /*
   * Need to predeclare these classes
   * I found that out the hard way
   */
  class MilesPerHour;
  class KilometersPerHour;
  class Knots;

class MetersPerSecond {

  friend MilesPerHour;
  friend KilometersPerHour;
  friend Knots;

 public:

  MetersPerSecond();

  MetersPerSecond(float temp);

  MetersPerSecond(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  bool operator==(const MetersPerSecond& other) const;

  bool operator!=(const MetersPerSecond& other) const;

  bool operator<(const MetersPerSecond& other) const;

  bool operator>(const MetersPerSecond& other) const;

  bool operator<=(const MetersPerSecond& other) const;

  bool operator>=(const MetersPerSecond& other) const;

  strong_ordering operator<=> (const MetersPerSecond& other) const;

  MetersPerSecond& operator=(const MetersPerSecond& other);

  MetersPerSecond& operator+=(const MetersPerSecond& other);

  MetersPerSecond& operator-=(const MetersPerSecond& other);

  const MetersPerSecond operator+(const MetersPerSecond& other) const;

  const MetersPerSecond operator-(const MetersPerSecond& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator MilesPerHour() const;

  operator KilometersPerHour() const;

  operator Knots() const;

 private:
  MetersPerSecond(int64_t base_value);

  int64_t base_value_;

  string fmt_value_ = speed_default_format;

  int MetersPerSecondToBase(float temp);

  float BaseToMetersPerSecond(int base);

  void setBaseValue(int base_value);
};

}  //qw_units Namespace

#endif  // LIB_UNITS_SPEED_METERS_PER_SECOND_H_
