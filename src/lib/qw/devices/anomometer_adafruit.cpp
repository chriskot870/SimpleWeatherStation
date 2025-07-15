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

#include "anomometer_adafruit.h"

using std::expected;
using std::unexpected;

using qw_devices::I2cAds1015;
using qw_devices::Ads1015MuxType;
using qw_devices::kAds1015CountPerVolts;
using qw_units::MilesPerHour;

namespace qw_devices {

  AnomometerAdafruit::AnomometerAdafruit(I2cAds1015 adc, Ads1015MuxType mux) : adc_(adc), mux_(mux) {}

  expected<MilesPerHour, int> AnomometerAdafruit::speed() {
    
    expected<int16_t, int> reading = adc_.getReading(mux_);
    if (reading.has_value() == false) {
      return unexpected(EIO);
    }

    /*
     * Convert the reading to a speed
     */

     float mph = 0.0;
     float count = reading.value();

     if (count > kAnomometerAdafruitBaseValue ) {
        mph = (count - kAnomometerAdafruitBaseValue) * kAnomometerAdafruitMphPerCount;
     }

    MilesPerHour speed(mph);

    return speed;
}

}  // qw_devices namespace