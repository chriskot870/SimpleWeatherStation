#include "inches_mercury.h"

namespace qw_units {

InchesMercury::InchesMercury() {}

InchesMercury::InchesMercury(float temp){

  base_value_ = MillibarToBase(temp);

  return;
}

float InchesMercury::getValue() {

    return BaseToMillibar(base_value_);
}

InchesMercury::operator Millibar() const {

  return Millibar(base_value_);
}
}