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

#include "qw/units/temperature/include/celsius.h"
#include "qw/units/temperature/include/fahrenheit.h"
#include "qw/units/temperature/include/kelvin.h"
#include "qw/units/temperature/include/temperature.h"

using qw::units::Celsius;
using qw::units::Fahrenheit;
using qw::units::Kelvin;
using qw::units::Temperature;
using std::string_view;

TEST(UnitsTemperatureTest, Assignment_Tests) {
  int resolution_factor = 1000;
  float celsius_value = 25.0;
  string_view celsius_str("25.00");
  float fahrenheit_value = 77.0;
  string_view fahrenheit_str("77.00");
  float kelvin_value = 298.15;
  string_view kelvin_str("298.15");

  // Test Celsius
  Celsius celsius(celsius_value);
  /*
     * Make sure they are the same within 3 decimal places
     */
  EXPECT_EQ(trunc(celsius.value() * resolution_factor),
            trunc(celsius_value * resolution_factor));
  EXPECT_EQ(celsius.toString(), celsius_str.data());

  // Test Fahrenheit
  Fahrenheit fahrenheit(fahrenheit_value);
  /*
     * Make sure they are the same within 3 decimal places
     */
  EXPECT_EQ(trunc(fahrenheit.value() * resolution_factor),
            trunc(fahrenheit_value * resolution_factor));
  EXPECT_EQ(fahrenheit.toString(), fahrenheit_str.data());

  // Test Kelvin
  Kelvin kelvin(kelvin_value);
  /*
     * Make sure they are the same within 3 decimal places
     */
  EXPECT_EQ(trunc(kelvin.value() * resolution_factor),
            trunc(kelvin_value * resolution_factor));
  EXPECT_EQ(kelvin.toString(), kelvin_str.data());
}

TEST(UnitsTemperatureTest, Casting_Tests) {
  /*
   * Cast to different unit and then cast back and see if it is the same
   */

  Celsius celsius_1(25.0);
  Fahrenheit fahrenheit_1 = celsius_1;  // Implicit cast to Fahrenheit
  Celsius celsius_2 = fahrenheit_1;     // Implicit cast back to
  EXPECT_EQ(celsius_1, celsius_2);      // Make sure they are equal
}

TEST(UnitsTemperatureTest, Equivalence_Tests) {
  Celsius c1(25.0);
  Fahrenheit f1 = c1;  // Implicit cast to Fahrenheit
  EXPECT_EQ(c1, f1);   // Make sure they are equal

  Celsius c2(25.0);
  Kelvin k1 = c2;     // Implicit cast to Kelvin
  EXPECT_EQ(c2, k1);  // Make sure they are equal

  Fahrenheit f2(77.0);
  Celsius c3 = f2;    // Implicit cast to Celsius
  EXPECT_EQ(f2, c3);  // Make sure they are equal

  Fahrenheit f3(77.0);
  Kelvin k2 = f3;     // Implicit cast to Kelvin
  EXPECT_EQ(f3, k2);  // Make sure they are equal

  Kelvin k3(298.15);
  Celsius c4 = k3;    // Implicit cast to Celsius
  EXPECT_EQ(k3, c4);  // Make sure they are equal

  Kelvin k4(298.15);
  Fahrenheit f4 = k4;  // Implicit cast to Fahrenheit
  EXPECT_EQ(k4, f4);   // Make sure they are equal
}
