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

#include "qw/units/temperature/include/fahrenheit.h"

using std::string;
using std::strong_ordering;

namespace qw::units {

class Celsius;
class Kelvin;

/*
 * Constructor routines
 */
Fahrenheit::Fahrenheit(){};

Fahrenheit::Fahrenheit(float temp) {

  base_value_ = FahrenheitToBase(temp);

  return;
}

Fahrenheit::Fahrenheit(float temp, string fmt_value) {

  base_value_ = FahrenheitToBase(temp);

  fmt_value_ = fmt_value;

  return;
}

/*
 * Data manipulation routines
 */
float Fahrenheit::value() {

  return BaseToFahrenheit(base_value_);
}

int Fahrenheit::FahrenheitToBase(float temp) {

  float f = (((temp - 32) * 5) / 9) * temperature_base_conversion_factor;
  int value = round(f);

  return value;
}

float Fahrenheit::BaseToFahrenheit(int base) {

  float value =
      ((((float)base / temperature_base_conversion_factor) * 9) / 5) + 32;

  return value;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Fahrenheit::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Fahrenheit::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this specific instance
 */
void Fahrenheit::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

/*
 * Used by conversion routines for implicit cast
 */
void Fahrenheit::setBaseValue(int base_value) {
  base_value_ = base_value;

  return;
}

/*********************************
 * Operator functions
 *********************************/
/*
 * Comparison operators
 */
bool Fahrenheit::operator==(const Fahrenheit& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Fahrenheit::operator!=(const Fahrenheit& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Fahrenheit::operator<(const Fahrenheit& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool Fahrenheit::operator>(const Fahrenheit& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Fahrenheit::operator<=(const Fahrenheit& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Fahrenheit::operator>=(const Fahrenheit& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Fahrenheit::operator<=> (const Fahrenheit& other) const {

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
Fahrenheit& Fahrenheit::operator=(const Fahrenheit& other) {

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

/*
 * Arithmetic operators
 */
Fahrenheit& Fahrenheit::operator+=(const Fahrenheit& other) {

  base_value_ += other.base_value_;

  return *this;
}

Fahrenheit& Fahrenheit::operator-=(const Fahrenheit& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Fahrenheit Fahrenheit::operator+(const Fahrenheit& other) const {

  Fahrenheit result = *this;

  result += other;

  return result;
}

const Fahrenheit Fahrenheit::operator-(const Fahrenheit& other) const {

  Fahrenheit result = *this;

  result -= other;

  return result;
}

/*
 * Conversion operators used for implicit casting
 */
Fahrenheit::operator Celsius() const {

  Celsius tempc;

  tempc.base_value_ = base_value_;

  return tempc;
}

Fahrenheit::operator Kelvin() const {

  Kelvin tempk;

  tempk.base_value_ = base_value_;

  return tempk;
}

}  // Namespace qw_units
