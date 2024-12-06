#include "inches_mercury.h"

namespace qw_units {

InchesMercury::InchesMercury() {}

InchesMercury::InchesMercury(float temp){

  base_value_ = MillibarToBase(temp);

  return;
}

InchesMercury::InchesMercury(float temp, string fmt_value) {

  base_value_ = InchesMercuryToBase(temp);

  fmt_value_ = fmt_value;

  return;
}

float InchesMercury::value() {

    return BaseToInchesMercury(base_value_);
}

InchesMercury::operator Millibar() const {

  Millibar mb;

  mb.base_value_ = base_value_;

  return mb;
}

}  // Namespace qw_units
