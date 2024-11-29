/*
 *
 */
#include "pressure.h"

namespace qw_units {

Pressure::Pressure(){}

Pressure::Pressure(int base_value) : base_value_(base_value) {}

auto Pressure::operator<=>(const Pressure& other) const {

  /*
   * The <=> returns a std::strong_ordering type.
   * Either ::less, ::equal, or ::greater
   */
  strong_ordering value = (base_value_ <=> other.base_value_);

  return value;
}

auto Pressure::operator==(const Pressure& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

auto Pressure::operator!=(const Pressure& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

auto Pressure::operator<(const Pressure& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;

}

auto Pressure::operator>(const Pressure& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

auto Pressure::operator<=(const Pressure& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

auto Pressure::operator>=(const Pressure& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}
   
/*
 * I am using a base of millicelsius
 */
int Pressure::InchesMercuryToBase(float inhg) {

  int value = round(((inhg * mb_sea_level)/inHg_sea_level) * pressure_base_conversion_factor);

  return value;
}

float Pressure::BaseToInchesMercury(int base) {

  float temp = (((float)base / pressure_base_conversion_factor) * inHg_sea_level)/mb_sea_level;

  return temp;
}

int Pressure::MillibarToBase(float mb) {

  int  value = round(mb * pressure_base_conversion_factor);

  return value;
}

float Pressure::BaseToMillibar(int base) {

  float value = (float)base/pressure_base_conversion_factor;

  return value;
}

}