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

#ifndef LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_
#define LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>
#include <variant>

#include "celsius.h"
#include "kelvin.h"
#include "temperature.h"

using fmt::format;
using std::string;
using std::strong_ordering;

namespace qw_units {
/*
 * Need to predeclare the friend classes
 * I found that out the hard way
 */

class Celsius;
class Kelvin;

class Fahrenheit {
  friend Celsius;
  friend Kelvin;

 public:
  Fahrenheit();

  Fahrenheit(float temp);

  Fahrenheit(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  /********************
   * Operator functions
   ********************/

  bool operator==(const Fahrenheit& other) const;

  bool operator!=(const Fahrenheit& other) const;

  bool operator<(const Fahrenheit& other) const;

  bool operator>(const Fahrenheit& other) const;

  bool operator<=(const Fahrenheit& other) const;

  bool operator>=(const Fahrenheit& other) const;

  strong_ordering operator<=> (const Fahrenheit& other) const;

  Fahrenheit& operator=(const Fahrenheit& other);

  Fahrenheit& operator+=(const Fahrenheit& other);

  Fahrenheit& operator-=(const Fahrenheit& other);

  const Fahrenheit operator+(const Fahrenheit& other) const;

  const Fahrenheit operator-(const Fahrenheit& other) const;

  operator Celsius() const;

  operator Kelvin() const;

 private:
  int64_t base_value_;

  string fmt_value_ = temperature_default_format;

  int FahrenheitToBase(float temp);

  float BaseToFahrenheit(int base);

  void setBaseValue(int base_value);
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_