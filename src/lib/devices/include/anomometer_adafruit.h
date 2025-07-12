/*
 * Copyright 2024 Chris Kottaridis
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
constexpr float kAnomometerAdafruitMphPerCount = kAnomometerAdafruitCalibrateMph/(kAds1015CountPerVolts * (kAnonometerAdafruitCalibrateVoltage - kAnonometerAdafruitBaseVolts));
constexpr float kAnomometerAdafruitBaseValue = (kAds1015CountPerVolts * kAnonometerAdafruitBaseVolts);

/*
 * volts = (volts/count) * count + b
 */

class AnomometerAdafruit {
 public:
  AnomometerAdafruit(I2cAds1015 adc, Ads1015MuxType mux);

  expected<MilesPerHour, int> speed();

 private:
  I2cAds1015 adc_;

  Ads1015MuxType mux_;

};

}

#endif  // SRC_LIB_DEVICES_WIND_SPEED_SENSOR_ADC_H_
