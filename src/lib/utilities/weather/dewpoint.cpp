

#include "dewpoint.h"

using qw_units::Celsius;
using qw_units::RelativeHumidity;

Celsius dewPoint(Celsius tempc, RelativeHumidity rh) {

  float val = log(rh.value()/100) + ((dew_point_b * tempc.value())/(dew_point_c + tempc.value()));

  float td = (dew_point_c * val)/(dew_point_b - val);

  Celsius dewptc(td);

  return dewptc;
}