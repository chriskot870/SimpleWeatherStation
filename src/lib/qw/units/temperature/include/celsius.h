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

#ifndef LIB_UNITS_TEMPERATURE_CELSIUS_H_
#define LIB_UNITS_TEMPERATURE_CELSIUS_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>
#include <variant>

#include "qw/units/temperature/include/fahrenheit.h"
#include "qw/units/temperature/include/kelvin.h"
#include "qw/units/temperature/include/temperature.h"

namespace qw::units {

/*
 * Need to predeclare the friend classes
 * I found that out the hard way
 */
class Fahrenheit;
class Kelvin;

class Celsius {

  friend Fahrenheit;
  friend Kelvin;

 public:
  Celsius();

  Celsius(float temp);

  Celsius(float temp, std::string fmt_value);

  bool operator==(const Celsius& other) const;

  bool operator!=(const Celsius& other) const;

  bool operator<(const Celsius& other) const;

  bool operator>(const Celsius& other) const;

  bool operator<=(const Celsius& other) const;

  bool operator>=(const Celsius& other) const;

  std::strong_ordering operator<=> (const Celsius& other) const;

  Celsius& operator=(const Celsius& other);

  Celsius& operator+=(const Celsius& other);

  Celsius& operator-=(const Celsius& other);

  const Celsius operator+(const Celsius& other) const;

  const Celsius operator-(const Celsius& other) const;

  /*
   * These constructors are for casting
   */
  operator Fahrenheit() const;

  operator Kelvin() const;

  //Celsius& operator=(const Celsius& other);

  float value();

  std::string toString();

  std::string toString(std::string format);

  void setFormat(std::string fmt_value);

 private:
  int64_t base_value_;

  std::string fmt_value_ = temperature_default_format;

  /*
   * Here we want all the conversion to and from base to the
   */
  int CelsiusToBase(float temp);

  float BaseToCelsius(int base);

  /*
   * USed for conversion for implicit casting
   */
  void setBaseValue(int base_value);
};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_CELSIUS_H_