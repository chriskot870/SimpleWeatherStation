
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

#include "qw/units/temperature/include/celsius.h"

using std::string;
using std::strong_ordering;

namespace qw::units {

class Kelvin;
class Fahrenheit;

/*
 * Constructor routines
 */
Celsius::Celsius() {}

Celsius::Celsius(float temp) {

  base_value_ = CelsiusToBase(temp);

  return;
}

Celsius::Celsius(float temp, string fmt_value) {

  base_value_ = CelsiusToBase(temp);

  fmt_value_ = fmt_value;

  return;
}

/*
 * Data manipulation routines
 */
float Celsius::value() {

  return BaseToCelsius(base_value_);
}

int Celsius::CelsiusToBase(float temp) {

  float f = temp * temperature_base_conversion_factor;
  int value = round(f);

  return value;
}

float Celsius::BaseToCelsius(int base) {

  float temp = (float)base / temperature_base_conversion_factor;

  return temp;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Celsius::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Celsius::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format to use for this instanace
 */
void Celsius::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

/*
 * Used by conversion functions to implement implicit cast between friends
 */
void Celsius::setBaseValue(int base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Comparison operators
 */
bool Celsius::operator==(const Celsius& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Celsius::operator!=(const Celsius& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Celsius::operator<(const Celsius& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool Celsius::operator>(const Celsius& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Celsius::operator<=(const Celsius& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Celsius::operator>=(const Celsius& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Celsius::operator<=> (const Celsius& other) const {

  /*
   * The <=> returns a std::strong_ordering type.
   * Either ::less, ::equal, or ::greater
   */
  strong_ordering value = (base_value_ <=> other.base_value_);

  return value;
}

/*
 * Assignment operators
 */
Celsius& Celsius::operator=(const Celsius& other) {

  /*
   * Guard against self assignement
   */
  if (this == &other) {
    return *this;
  }

  /*
   * This is the one that is to the left of = sign so we eant
   * to copy the base_value_ in other to the one in this
   */
  base_value_ = other.base_value_;

  return *this;
}

Celsius& Celsius::operator+=(const Celsius& other) {

  base_value_ += other.base_value_;

  return *this;
}

Celsius& Celsius::operator-=(const Celsius& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Celsius Celsius::operator+(const Celsius& other) const {

  Celsius result = *this;

  result += other;

  return result;
}

const Celsius Celsius::operator-(const Celsius& other) const {

  Celsius result = *this;

  result -= other;

  return result;
}

/*
 * Conversion routines for implicit casting to friends
 */
Celsius::operator Kelvin() const {

  Kelvin tempk;

  tempk.setBaseValue(base_value_);

  return tempk;
}

Celsius::operator Fahrenheit() const {

  Fahrenheit tempf;

  tempf.setBaseValue(base_value_);

  return tempf;
}

}  // Namespace qw_units
