/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_SPEED_KILOMETERS_PER_HOUR_H_
#define LIB_UNITS_SPEED_KILOMETERS_PER_HOUR_H_

#include "miles_per_hour.h"
#include "meters_per_second.h"
#include "knots.h"
#include "speed.h"

namespace qw_units {

/*
 * Need to predeclare these classes
 * I found that out the hard way
 */
class MilesPerHour;
class MetersPerSecond;
class Knots;

class KilometersPerHour {

  friend MilesPerHour;
  friend MetersPerSecond;
  friend Knots;

  public:
  KilometersPerHour();

  KilometersPerHour(float temp);

  KilometersPerHour(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  bool operator==(const KilometersPerHour& other) const;

  bool operator!=(const KilometersPerHour& other) const;

  bool operator<(const KilometersPerHour& other) const;

  bool operator>(const KilometersPerHour& other) const;

  bool operator<=(const KilometersPerHour& other) const;

  bool operator>=(const KilometersPerHour& other) const;

  strong_ordering operator<=> (const KilometersPerHour& other) const;

  KilometersPerHour& operator=(const KilometersPerHour& other);

  KilometersPerHour& operator+=(const KilometersPerHour& other);

  KilometersPerHour& operator-=(const KilometersPerHour& other);

  const KilometersPerHour operator+(const KilometersPerHour& other) const;

  const KilometersPerHour operator-(const KilometersPerHour& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator MilesPerHour() const;

  operator MetersPerSecond() const;

  operator Knots() const;

 private:
  int64_t base_value_;

  string fmt_value_ = speed_default_format;

  int KilometersPerHourToBase(float temp);

  float BaseToKilometersPerHour(int base);

  void setBaseValue(int base_value);
};

}  //qw_units Namespace

#endif  // LIB_UNITS_SPEED_KILOMETERS_PER_HOUR_H_
