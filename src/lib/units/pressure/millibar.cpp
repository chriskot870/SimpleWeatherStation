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

/*********************
 * Operator routines
 *********************/
/*
 * Comparison operators
 */

bool Millibar::operator==(const Millibar& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Millibar::operator!=(const Millibar& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Millibar::operator<(const Millibar& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;

}

bool Millibar::operator>(const Millibar& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Millibar::operator<=(const Millibar& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Millibar::operator>=(const Millibar& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Millibar::operator<=>(const Millibar& other) const {

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
Millibar& Millibar::operator=(const Millibar& other) {

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

Millibar& Millibar::operator+=(const Millibar& other) {

  base_value_ += other.base_value_;

  return *this;
}

Millibar& Millibar::operator-=(const Millibar& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Millibar Millibar::operator+(const Millibar &other) const {

  Millibar result = *this;

  result += other;

  return result;
}

const Millibar Millibar::operator-(const Millibar &other) const {

  Millibar result = *this;

  result -= other;

  return result;
}

int Millibar::MillibarToBase(float mb) {

  int  value = round(mb * pressure_base_conversion_factor);

  return value;
}

float Millibar::BaseToMillibar(int base) {

  float value = (float)base/pressure_base_conversion_factor;

  return value;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Millibar::toString() {

  string data = format(fmt::runtime(fmt_value_), base_value_);

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Millibar::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), base_value_);

  return data;
}

void Millibar::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

void Millibar::setBaseValue(int base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Conversion routines for implicit casting
 */
Millibar::operator InchesMercury() const {
  
  InchesMercury inhg;

  inhg.base_value_ = base_value_;

  return inhg;
}

}  // Namespace qw_units

