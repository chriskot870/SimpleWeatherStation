/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_SPEED_MILES_PER_HOUR_H_
#define LIB_UNITS_SPEED_MILES_PER_HOUR_H_


#include "speed.h"
#include "kilometers_per_hour.h"
#include "meters_per_second.h"
#include "knots.h"


namespace qw_units {

  /*
   * Need to predeclare these classes
   * I found that out the hard way
   */
  class KilometersPerHour;
  class MetersPerSecond;
  class Knots;

class MilesPerHour {

  friend KilometersPerHour;
  friend MetersPerSecond;
  friend Knots;

  public:
  MilesPerHour();

  MilesPerHour(float temp);

  MilesPerHour(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  bool operator==(const MilesPerHour& other) const;

  bool operator!=(const MilesPerHour& other) const;

  bool operator<(const MilesPerHour& other) const;

  bool operator>(const MilesPerHour& other) const;

  bool operator<=(const MilesPerHour& other) const;

  bool operator>=(const MilesPerHour& other) const;

  strong_ordering operator<=> (const MilesPerHour& other) const;

  MilesPerHour& operator=(const MilesPerHour& other);

  MilesPerHour& operator+=(const MilesPerHour& other);

  MilesPerHour& operator-=(const MilesPerHour& other);

  const MilesPerHour operator+(const MilesPerHour& other) const;

  const MilesPerHour operator-(const MilesPerHour& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator KilometersPerHour() const;

  operator MetersPerSecond() const;

  operator Knots() const;

 private:
  int64_t base_value_;

  string fmt_value_ = speed_default_format;
  
  MilesPerHour(int64_t base_value);

  int MilesPerHourToBase(float temp);

  float BaseToMilesPerHour(int base);

  void setBaseValue(int base_value);
};

}  //qw_units Namespace

#endif  // LIB_UNITS_SPEED_MILES_PER_HOUR_H_