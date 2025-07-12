#include "anomometer_adafruit.h"

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