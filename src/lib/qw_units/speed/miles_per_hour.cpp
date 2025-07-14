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

#include "miles_per_hour.h"

using std::string;
using std::strong_ordering;

namespace qw_units {

class KilometersPerHour;
class MetersPerSecond;
class Knots;

/*
 * Constructor routines
 */
MilesPerHour::MilesPerHour() {}

MilesPerHour::MilesPerHour(float mph) : base_value_(MilesPerHourToBase(mph)) {}

MilesPerHour::MilesPerHour(float mph, string fmt_value)
    : base_value_(MilesPerHourToBase(mph)), fmt_value_(fmt_value) {}

MilesPerHour::MilesPerHour(int64_t base_value) : base_value_(base_value) {}

/*
 * Data manipulation routines
 */
float MilesPerHour::value() {

  return BaseToMilesPerHour(base_value_);
}

/*
 * For every mile per hour there are 100 base units.
 */
int MilesPerHour::MilesPerHourToBase(float mph) {

  int value = round(mph * speed_base_conversion_factor);

  return value;
}

float MilesPerHour::BaseToMilesPerHour(int base) {

  float mph = ((float)base / speed_base_conversion_factor);

  return mph;
}

/*
 * Use the default format
 */
string MilesPerHour::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format instead of one in private variable
 */
string MilesPerHour::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this instance
 */
void MilesPerHour::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

/*
 * Used by conversion routines for implicit casting
 */
void MilesPerHour::setBaseValue(int base_value) {

  base_value_ = base_value;

  return;
}

bool MilesPerHour::operator==(const MilesPerHour& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool MilesPerHour::operator!=(const MilesPerHour& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool MilesPerHour::operator<(const MilesPerHour& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool MilesPerHour::operator>(const MilesPerHour& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool MilesPerHour::operator<=(const MilesPerHour& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool MilesPerHour::operator>=(const MilesPerHour& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering MilesPerHour::operator<=> (const MilesPerHour& other) const {

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
MilesPerHour& MilesPerHour::operator=(const MilesPerHour& other) {

  /*
   * Guard against self assignement
   */
  if (this == &other) {
    return *this;
  }

  /*
   * This is the one that is to the left of = sign so we want
   * to copy the base_value_ in other to the one in this
   */
  base_value_ = other.base_value_;

  return *this;
}

MilesPerHour& MilesPerHour::operator+=(const MilesPerHour& other) {

  base_value_ += other.base_value_;

  return *this;
}

MilesPerHour& MilesPerHour::operator-=(const MilesPerHour& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const MilesPerHour MilesPerHour::operator+(const MilesPerHour& other) const {

  MilesPerHour result = *this;

  result += other;

  return result;
}

const MilesPerHour MilesPerHour::operator-(const MilesPerHour& other) const {

  MilesPerHour result = *this;

  result -= other;

  return result;
}

/*
 * Intrinsic casting to other speed units
 */

MilesPerHour::operator KilometersPerHour() const {

  KilometersPerHour kph(base_value_);

  return kph;
}

MilesPerHour::operator MetersPerSecond() const {

  MetersPerSecond mps(base_value_);

  return mps;
}

MilesPerHour::operator Knots() const {

  Knots knots(base_value_);

  return knots;
}

}  // namespace qw_units
