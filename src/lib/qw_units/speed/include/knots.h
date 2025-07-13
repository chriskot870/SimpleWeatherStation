/*
 * Copyright 2024,2025, Chris Kottaridis. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * NTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
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
