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
#include <gtest/gtest.h>

#include <cmath>
#include <string_view>

#include "qw/units/pressure/include/millibar.h"
#include "qw/units/pressure/include/inches_mercury.h"
#include "qw/units/pressure/include/pressure.h"

using qw::units::InchesMercury;
using qw::units::Millibar;
using qw::units::Pressure;
using std::string_view;

TEST(UnitsPressureTest, Assignment_Tests) {
  int resolution_factor = 1000;
  float inches_value = 25.0;
  string_view inches_str("25.00");
  float millibar_value = 1013.25;
  string_view millibar_str("1013.25");


  // Test Inches Mercury
  InchesMercury inches(inches_value);
  /*
     * Make sure they are the same within 3 decimal places
     */
  EXPECT_EQ(trunc(inches.value() * resolution_factor),
            trunc(inches_value * resolution_factor));
  EXPECT_EQ(inches.toString(), inches_str.data());

  // Test Millibar
  Millibar millibar(millibar_value);
  /*
     * Make sure they are the same within 3 decimal places
     */
  EXPECT_EQ(trunc(millibar.value() * resolution_factor),
            trunc(millibar_value * resolution_factor));
  EXPECT_EQ(millibar.toString(), millibar_str.data());
}

TEST(UnitsPressureTest, Casting_Tests) {
  /*
   * Cast to different unit and then cast back and see if it is the same
   */

  InchesMercury inches_1(25.0);
  Millibar millibar_1 = inches_1;  // Implicit cast to Fahrenheit
  InchesMercury inches_2 = millibar_1;     // Implicit cast back to
  EXPECT_EQ(inches_1, inches_2);      // Make sure they are equal
}

TEST(UnitsPressureTest, Equivalence_Tests) {
  InchesMercury i1(25.0);
  Millibar m1 = i1;  // Implicit cast to Fahrenheit
  EXPECT_EQ(i1, m1);   // Make sure they are equal

  Millibar m2(1013.25);
  InchesMercury i3 = m2;    // Implicit cast to Celsius
  EXPECT_EQ(m2, i3);  // Make sure they are equal
}
