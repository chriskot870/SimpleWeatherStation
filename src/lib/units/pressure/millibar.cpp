#include "millibar.h"

namespace qw_units {

Millibar::Millibar() {}

Millibar::Millibar(float mb){

  base_value_ = MillibarToBase(mb);

  return;
}

Millibar::Millibar(float mb, string fmt_value) {

  base_value_ = MillibarToBase(mb);

  fmt_value_ = fmt_value;

  return;
}

float Millibar::value() {

    return BaseToMillibar(base_value_);
}

Millibar::operator InchesMercury() const {
  
  InchesMercury inhg;

  inhg.base_value_ = base_value_;

  return inhg;
}

}  // Namespace qw_units

