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

#ifndef SRC_LIB_QW_UNITS_PRESSURE_INCLUDE_INCHES_MERCURY_H_
#define SRC_LIB_QW_UNITS_PRESSURE_INCLUDE_INCHES_MERCURY_H_

#include "qw/units/pressure/include/millibar.h"
#include "qw/units/pressure/include/pressure.h"

namespace qw::units {

class InchesMercury : public Pressure {

 public:
  InchesMercury();

  InchesMercury(float temp);

  InchesMercury(float temp, std::string fmt_value);

  float value();

  std::string toString();

  std::string toString(std::string format);

  void setFormat(std::string fmt_value);

  bool operator==(const InchesMercury& other) const;

  bool operator!=(const InchesMercury& other) const;

  bool operator<(const InchesMercury& other) const;

  bool operator>(const InchesMercury& other) const;

  bool operator<=(const InchesMercury& other) const;

  bool operator>=(const InchesMercury& other) const;

  std::strong_ordering operator<=> (const InchesMercury& other) const;

  InchesMercury& operator=(const InchesMercury& other);

  InchesMercury& operator+=(const InchesMercury& other);

  InchesMercury& operator-=(const InchesMercury& other);

  const InchesMercury operator+(const InchesMercury& other) const;

  const InchesMercury operator-(const InchesMercury& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator Millibar() const;

 private:

  std::string fmt_value_ = pressure_default_format;

  int InchesMercuryToBase(float temp);

  float BaseToInchesMercury(int base);

};

}  // namespace qw::units

#endif  // SRC_LIB_QW_UNITS_PRESSURE_INCLUDE_INCHES_MERCURY_H_
