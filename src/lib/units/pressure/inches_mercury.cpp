#include "inches_mercury.h"

namespace qw_units {

InchesMercury::InchesMercury() {}

InchesMercury::InchesMercury(float temp){

  base_value_ = InchesMercuryToBase(temp);

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

/*
 * I am using a base of millicelsius
 */
int InchesMercury::InchesMercuryToBase(float inhg) {

  int value = round(((inhg * mb_sea_level)/inHg_sea_level) * pressure_base_conversion_factor);

  return value;
}

float InchesMercury::BaseToInchesMercury(int base) {

  float inhg = (((float)base / pressure_base_conversion_factor) * inHg_sea_level)/mb_sea_level;

  return inhg;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string InchesMercury::toString() {

  string data = format(fmt::runtime(fmt_value_), base_value_);

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string InchesMercury::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), base_value_);

  return data;
}

void InchesMercury::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

void InchesMercury::setBaseValue(int base_value) {
  base_value_ = base_value;

  return;
}


bool InchesMercury::operator==(const InchesMercury& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool InchesMercury::operator!=(const InchesMercury& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool InchesMercury::operator<(const InchesMercury& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;

}

bool InchesMercury::operator>(const InchesMercury& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool InchesMercury::operator<=(const InchesMercury& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool InchesMercury::operator>=(const InchesMercury& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering InchesMercury::operator<=>(const InchesMercury& other) const {

  /*
   * The <=> returns a std::strong_ordering type.
   * Either ::less, ::equal, or ::greater
   */
  strong_ordering value = (base_value_ <=> other.base_value_);

  return value;
}

/*
 * Assignment operators
 */
InchesMercury& InchesMercury::operator=(const InchesMercury& other) {

  /*
   * Guard against self assignement
   */
  if (this == &other) {
    return *this;
  }

  /*
   * This is the one that is to the left of = sign so we eant
   * to copy the base_value_ in other to the one in this
   */
  base_value_ = other.base_value_;

  return *this;
}

InchesMercury& InchesMercury::operator+=(const InchesMercury& other) {

  base_value_ += other.base_value_;

  return *this;
}

InchesMercury& InchesMercury::operator-=(const InchesMercury& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const InchesMercury InchesMercury::operator+(const InchesMercury &other) const {

  InchesMercury result = *this;

  result += other;

  return result;
}

const InchesMercury InchesMercury::operator-(const InchesMercury &other) const {

  InchesMercury result = *this;

  result -= other;

  return result;
}

InchesMercury::operator Millibar() const {

  Millibar mb;

  mb.base_value_ = base_value_;

  return mb;
}

}  // Namespace qw_units
