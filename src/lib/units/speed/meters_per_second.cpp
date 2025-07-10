#include "meters_per_second.h"

namespace qw_units {

  class MilesPerHour;
  class KilometersPerHour;
  class Knots;

/*
 * Constructor routines
 */
MetersPerSecond::MetersPerSecond() {}

MetersPerSecond::MetersPerSecond(float mps) : base_value_(MetersPerSecondToBase(mps)) {}

MetersPerSecond::MetersPerSecond(float mps, string fmt_value) :
  base_value_(MetersPerSecondToBase(mps)), fmt_value_(fmt_value) {}

MetersPerSecond::MetersPerSecond(int64_t base_value) : base_value_(base_value) {}

/*
 * Data manipulation routines
 */
float MetersPerSecond::value() {

  return BaseToMetersPerSecond(base_value_);
}

/*
 * For every mile per hour there are 100 base units.
 * Convert the meters per second to miles per hour.
 * Then convert the miles per hour to the base.
 */
int MetersPerSecond::MetersPerSecondToBase(float mps) {

  int value = round((mps * kMphPerMps) * speed_base_conversion_factor);

  return value;
}

/*
 * Convert the base to miles per hour.
 * Then convert the miles per hour to meters per second.
 */
float MetersPerSecond::BaseToMetersPerSecond(int base) {

  float mps = ((float)base / speed_base_conversion_factor) * kMpsPerMph;

  return mps;
}

/*
 * Use the default format
 */
string MetersPerSecond::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format instead of one in private variable
 */
string MetersPerSecond::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this instance
 */
void MetersPerSecond::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

/*
 * Used by conversion routines for implicit casting
 */
void MetersPerSecond::setBaseValue(int base_value) {
  
  base_value_ = base_value;

  return;
}

bool MetersPerSecond::operator==(const MetersPerSecond& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool MetersPerSecond::operator!=(const  MetersPerSecond& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool MetersPerSecond::operator<(const MetersPerSecond& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool MetersPerSecond::operator>(const MetersPerSecond& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool MetersPerSecond::operator<=(const MetersPerSecond& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool MetersPerSecond::operator>=(const MetersPerSecond& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering MetersPerSecond::operator<=> (const MetersPerSecond& other) const {

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
MetersPerSecond& MetersPerSecond::operator=(const MetersPerSecond& other) {

  /*
   * Guard against self assignement
   */
  if (this == &other) {
    return *this;
  }

  /*
   * This is the one that is to the left of = sign so we want
   * to copy the base_value_ in other to the one in this
   */
  base_value_ = other.base_value_;

  return *this;
}

MetersPerSecond& MetersPerSecond::operator+=(const MetersPerSecond& other) {

  base_value_ += other.base_value_;

  return *this;
}

MetersPerSecond& MetersPerSecond::operator-=(const MetersPerSecond& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const MetersPerSecond MetersPerSecond::operator+(const MetersPerSecond& other) const {

  MetersPerSecond result = *this;

  result += other;

  return result;
}

const MetersPerSecond MetersPerSecond::operator-(const MetersPerSecond& other) const {

  MetersPerSecond result = *this;

  result -= other;

  return result;
}

/*
 * Intrinsic casting to other speed units
 */
MetersPerSecond::operator KilometersPerHour() const {

  KilometersPerHour kph(base_value_);

  return kph;
}

MetersPerSecond::operator MilesPerHour() const {

  MilesPerHour mph(base_value_);

  return mph;
}

MetersPerSecond::operator Knots() const {

  Knots knots(base_value_);

  return knots;
}

}  // qw_units Namespace
