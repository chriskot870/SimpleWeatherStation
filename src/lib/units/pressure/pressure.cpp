/*
 *
 */
#include "pressure.h"

namespace qw_units {

Pressure::Pressure() {}

void Pressure::setBaseValue(int base_value) {
  
  base_value_ = base_value;

  return;
}

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

  float inhg = (((float)base / pressure_base_conversion_factor) * inHg_sea_level)/mb_sea_level;

  return inhg;
}

int Pressure::MillibarToBase(float mb) {

  int  value = round(mb * pressure_base_conversion_factor);

  return value;
}

float Pressure::BaseToMillibar(int base) {

  float value = (float)base/pressure_base_conversion_factor;

  return value;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Pressure::toString() {

  string data = format(fmt::runtime(fmt_value_), base_value_);

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Pressure::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), base_value_);

  return data;
}

void Pressure::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

}  // Namespace qw_units
