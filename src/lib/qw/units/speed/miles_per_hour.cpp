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
#include "qw/units/speed/include/miles_per_hour.h"

#include <format>
#include <math.h>

#include <compare>
#include <string>

using std::format;
using std::vformat;
using std::make_format_args;
using std::string;
using std::strong_ordering;

namespace qw::units {

/*
 * Constructor routines
 */
MilesPerHour::MilesPerHour() {}

MilesPerHour::MilesPerHour(float mph) : Speed(milesPerHourToBase(mph)) {}

MilesPerHour::MilesPerHour(float mph, string fmt_value)
    : Speed(milesPerHourToBase(mph)), fmt_value_(fmt_value) {}

/*
 * Data manipulation routines
 */
float MilesPerHour::value() {
  return baseToMilesPerHour(base_value_);
}

/*
 * For every mile per hour there are speed_base_conversion_factor base units.
 */
int MilesPerHour::milesPerHourToBase(float mph) {
  int value = round(mph * speed_base_conversion_factor);

  return value;
}

float MilesPerHour::baseToMilesPerHour(int64_t base) {
  float mph = (static_cast<float>(base) / speed_base_conversion_factor);

  return mph;
}

void MilesPerHour::setBase(int64_t base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Use the default format
 */
string MilesPerHour::toString() {
  float i = value();
  string data = vformat(fmt_value_, make_format_args(i));

  return data;
}

/*
 * Use the provided format instead of one in private variable
 */
string MilesPerHour::toString(string fmt_value) {
  float i = value();
  string data = vformat(fmt_value, make_format_args(i));

  return data;
}

/*
 * Set the format for this instance
 */
void MilesPerHour::setFormat(string fmt_value) {
  fmt_value_ = fmt_value;

  return;
}

}  // namespace qw::units
