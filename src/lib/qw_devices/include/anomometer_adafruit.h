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

/*
 * This the driver for the Ada Fruit Wind Speed Sensor.
 * See https://www.adafruit.com/product/1733. It uses
 * Analog output to identigfy the wind speed. It provides
 * a .4 Volt value when it is not spinning. The data
 * sheet on Ada Fruit says it provides 2.0V when wind speed
 * is 72 mph. I am assume that is not linear so determing
 * how to map the speed to voltage will have to be trial
 * and error.
 */

#ifndef SRC_LIB_DEVICES_WIND_SPEED_SENSOR_ADC_H_
#define SRC_LIB_DEVICES_WIND_SPEED_SENSOR_ADC_H_

#include "ads1015.h"
#include "miles_per_hour.h"
#include "speed.h"

using qw_devices::I2cAds1015;
using qw_devices::Ads1015MuxType;
using qw_devices::kAds1015CountPerVolts;
using qw_units::MilesPerHour;

namespace qw_devices {

constexpr float kAnonometerAdafruitBaseVolts = .4;
constexpr float kAnonometerAdafruitCalibrateVoltage = 2.0;
constexpr float kAnomometerAdafruitCalibrateMph = 72;
constexpr float kAnomometerAdafruitMphPerCount = kAnomometerAdafruitCalibrateMph/(qw_devices::kAds1015CountPerVolts * (kAnonometerAdafruitCalibrateVoltage - kAnonometerAdafruitBaseVolts));
constexpr float kAnomometerAdafruitBaseValue = (qw_devices::kAds1015CountPerVolts * kAnonometerAdafruitBaseVolts);

/*
 * volts = (volts/count) * count + b
 */

class AnomometerAdafruit {
 public:
  AnomometerAdafruit(I2cAds1015 adc, Ads1015MuxType mux);

  std::expected<MilesPerHour, int> speed();

 private:
  qw_devices::I2cAds1015 adc_;

  qw_devices::Ads1015MuxType mux_;

};

}

#endif  // SRC_LIB_DEVICES_WIND_SPEED_SENSOR_ADC_H_
