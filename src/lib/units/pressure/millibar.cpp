#include "millibar.h"

namespace qw_units {

Millibar::Millibar() {}

Millibar::Millibar(float temp){

  base_value_ = MillibarToBase(temp);

  return;
}

float Millibar::getValue() {

    return BaseToMillibar(base_value_);
}

Millibar::operator InchesMercury() const {

  return InchesMercury(base_value_);
}
}
