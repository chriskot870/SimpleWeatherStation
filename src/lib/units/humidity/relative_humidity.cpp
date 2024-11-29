
#include "relative_humidity.h"

namespace qw_units {

RelativeHumidity::RelativeHumidity() {}

RelativeHumidity::RelativeHumidity(float rh) {

  base_value_ = round(rh * rh_base_conversion_factor);
}

/*
 * We want this just for operator=. I think we might be able to make this private
 */
RelativeHumidity::RelativeHumidity(int rh) : base_value_(rh) {}

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

float RelativeHumidity::getValue() {

  float value = ((float)base_value_ / rh_base_conversion_factor);

  return value;
}

}
