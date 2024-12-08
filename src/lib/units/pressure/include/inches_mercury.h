/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_PRESSURE_INCHES_MERCURY_H_
#define LIB_UNITS_PRESSURE_INCHES_MERCURY_H_

#include "pressure.h"
#include "millibar.h"

namespace qw_units {
/*
 * Need to predeclare these classes
 * I found that out the hard way
 */
class Millibar;

class InchesMercury {

  friend Millibar;

 public:
  InchesMercury();

  InchesMercury(float temp);

  InchesMercury(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  bool operator==(const InchesMercury& other) const;

  bool operator!=(const InchesMercury& other) const;

  bool operator<(const InchesMercury& other) const;

  bool operator>(const InchesMercury& other) const;

  bool operator<=(const InchesMercury& other) const;

  bool operator>=(const InchesMercury& other) const;

  strong_ordering operator<=>(const InchesMercury& other) const;

  InchesMercury& operator=(const InchesMercury& other);

  InchesMercury& operator+=(const InchesMercury& other);

  InchesMercury& operator-=(const InchesMercury& other);

  const InchesMercury operator+(const InchesMercury &other) const;

  const InchesMercury operator-(const InchesMercury &other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator Millibar() const;

  private:
   int64_t base_value_;

   string fmt_value_ = pressure_default_format;

   int InchesMercuryToBase(float temp);

   float BaseToInchesMercury(int base);

   void setBaseValue(int base_value);
};

}  // Namespace qw_units

#endif  // LIB_UNITS_PRESSURE_INCHES_MERCURY_H_