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

#include "inches_mercury.h"

using std::string;
using std::strong_ordering;

namespace qw_units {

/*
 * Constructor routines
 */
InchesMercury::InchesMercury() {}

InchesMercury::InchesMercury(float temp) {

  base_value_ = InchesMercuryToBase(temp);

  return;
}

InchesMercury::InchesMercury(float temp, string fmt_value) {

  base_value_ = InchesMercuryToBase(temp);

  fmt_value_ = fmt_value;

  return;
}

/*
 * Data manipulation routines
 */
float InchesMercury::value() {

  return BaseToInchesMercury(base_value_);
}

/*
 * I am using a base of milli-millibars as a base unit
 */
int InchesMercury::InchesMercuryToBase(float inhg) {

  int value = round(((inhg * mb_sea_level) / inHg_sea_level) *
                    pressure_base_conversion_factor);

  return value;
}

float InchesMercury::BaseToInchesMercury(int base) {

  float inhg =
      (((float)base / pressure_base_conversion_factor) * inHg_sea_level) /
      mb_sea_level;

  return inhg;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string InchesMercury::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string InchesMercury::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this instance
 */
void InchesMercury::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

/*
 * Used by conversion routines for implicit casting
 */
void InchesMercury::setBaseValue(int base_value) {
  base_value_ = base_value;

  return;
}

bool InchesMercury::operator==(const InchesMercury& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool InchesMercury::operator!=(const InchesMercury& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool InchesMercury::operator<(const InchesMercury& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool InchesMercury::operator>(const InchesMercury& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool InchesMercury::operator<=(const InchesMercury& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool InchesMercury::operator>=(const InchesMercury& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering InchesMercury::operator<=> (const InchesMercury& other) const {

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
InchesMercury& InchesMercury::operator=(const InchesMercury& other) {

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

InchesMercury& InchesMercury::operator+=(const InchesMercury& other) {

  base_value_ += other.base_value_;

  return *this;
}

InchesMercury& InchesMercury::operator-=(const InchesMercury& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const InchesMercury InchesMercury::operator+(const InchesMercury& other) const {

  InchesMercury result = *this;

  result += other;

  return result;
}

const InchesMercury InchesMercury::operator-(const InchesMercury& other) const {

  InchesMercury result = *this;

  result -= other;

  return result;
}

InchesMercury::operator Millibar() const {

  Millibar mb;

  mb.base_value_ = base_value_;

  return mb;
}

}  // Namespace qw_units
