
#include "relative_humidity.h"

namespace qw_units {

/*
 * Constructor routines
 */
RelativeHumidity::RelativeHumidity() {}

RelativeHumidity::RelativeHumidity(float rh) {

  base_value_ = round(rh * rh_base_conversion_factor);

  return;
}

RelativeHumidity::RelativeHumidity(float rh, string fmt_value) {

  base_value_ = round(rh * rh_base_conversion_factor);

  fmt_value_ = fmt_value;

  return;
}

/*
 * Data manipulation routnes
 */float RelativeHumidity::value() {

  float value = ((float)base_value_ / rh_base_conversion_factor);

  return value;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string RelativeHumidity::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string RelativeHumidity::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

void RelativeHumidity::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

void RelativeHumidity::setBaseValue(int base_value) {
  
  base_value_ = base_value;

  return;
}

bool RelativeHumidity::operator==(const RelativeHumidity& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool RelativeHumidity::operator!=(const RelativeHumidity& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool RelativeHumidity::operator<(const RelativeHumidity& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool RelativeHumidity::operator>(const RelativeHumidity& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool RelativeHumidity::operator<=(const RelativeHumidity& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool RelativeHumidity::operator>=(const RelativeHumidity& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering RelativeHumidity::operator<=>(const RelativeHumidity& other) const {

  strong_ordering value = (base_value_ <=> other.base_value_);

  return value;
}

RelativeHumidity& RelativeHumidity::operator=(const RelativeHumidity& other) {

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

RelativeHumidity& RelativeHumidity::operator+=(const RelativeHumidity& other) {

  base_value_ += other.base_value_;

  return *this;
}

RelativeHumidity& RelativeHumidity::operator-=(const RelativeHumidity& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const RelativeHumidity RelativeHumidity::operator+(const RelativeHumidity &other) const {

  RelativeHumidity result = *this;

  result += other;

  return result;
}

const RelativeHumidity RelativeHumidity::operator-(const RelativeHumidity &other) const {

  RelativeHumidity result = *this;

  result -= other;

  return result;
}

}  // namespace qw_units

