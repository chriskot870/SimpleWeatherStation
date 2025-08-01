
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
Celsius::Celsius() {}

Celsius::Celsius(float temp) : Temperature(celsiusToBase(temp)) {}

Celsius::Celsius(float temp, string fmt_value)
    : Temperature(celsiusToBase(temp)), fmt_value_(fmt_value) {}

/*
 * Data manipulation routines
 */
float Celsius::value() {
  return baseToCelsius(base_value_);
}

int64_t Celsius::celsiusToBase(float temp) {
  float f = temp * temperature_base_conversion_factor;
  int value = round(f);

  return value;
}

float Celsius::baseToCelsius(int base) {
  float temp = static_cast<float>(base) / temperature_base_conversion_factor;

  return temp;
}

void Celsius::setBase(int64_t base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Use the default format
 */
string Celsius::toString() {
  float i = value();
  string data = vformat(fmt_value_, make_format_args(i));

  return data;
}

/*
 * Use the provided format
 */
string Celsius::toString(string fmt_value) {
  float i = value();
  string data = vformat(fmt_value, make_format_args(i));

  return data;
}

/*
 * Set the format to use for this instanace
 */
void Celsius::setFormat(string fmt_value) {
  fmt_value_ = fmt_value;

  return;
}

}  // namespace qw::units
