#include "miles_per_hour.h"

namespace qw_units {

  class KilometersPerHour;
  class MetersPerSecond;
  class Knots;

/*
 * Constructor routines
 */
MilesPerHour::MilesPerHour() {}

MilesPerHour::MilesPerHour(float mph) : base_value_(MilesPerHourToBase(mph)) {}

MilesPerHour::MilesPerHour(float mph, string fmt_value) :
  base_value_(MilesPerHourToBase(mph)), fmt_value_(fmt_value) {}

/*
 * Data manipulation routines
 */
float MilesPerHour::value() {

  return BaseToMilesPerHour(base_value_);
}

/*
 * For every mile per hour there are 100 base units.
 */
int MilesPerHour::MilesPerHourToBase(float mph) {

  int value = round(mph * speed_base_conversion_factor);

  return value;
}

float MilesPerHour::BaseToMilesPerHour(int base) {

  float mph = ((float)base / speed_base_conversion_factor);

  return mph;
}

/*
 * Use the default format
 */
string MilesPerHour::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format instead of one in private variable
 */
string MilesPerHour::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this instance
 */
void MilesPerHour::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

/*
 * Used by conversion routines for implicit casting
 */
void MilesPerHour::setBaseValue(int base_value) {
  
  base_value_ = base_value;

  return;
}

bool MilesPerHour::operator==(const MilesPerHour& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool MilesPerHour::operator!=(const  MilesPerHour& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool MilesPerHour::operator<(const MilesPerHour& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool MilesPerHour::operator>(const MilesPerHour& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool MilesPerHour::operator<=(const MilesPerHour& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool MilesPerHour::operator>=(const MilesPerHour& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering MilesPerHour::operator<=> (const MilesPerHour& other) const {

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
MilesPerHour& MilesPerHour::operator=(const MilesPerHour& other) {

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

MilesPerHour& MilesPerHour::operator+=(const MilesPerHour& other) {

  base_value_ += other.base_value_;

  return *this;
}

MilesPerHour& MilesPerHour::operator-=(const MilesPerHour& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const MilesPerHour MilesPerHour::operator+(const MilesPerHour& other) const {

  MilesPerHour result = *this;

  result += other;

  return result;
}

const MilesPerHour MilesPerHour::operator-(const MilesPerHour& other) const {

  MilesPerHour result = *this;

  result -= other;

  return result;
}

/*
 * Intrinsic casting to other speed units
 */

MilesPerHour::operator KilometersPerHour() const {

  KilometersPerHour kph;

  kph.base_value_ = base_value_;

  return kph;
}

MilesPerHour::operator MetersPerSecond() const {

  MetersPerSecond mps;

  mps.base_value_ = base_value_;

  return mps;
}

MilesPerHour::operator Knots() const {

  Knots knots;

  knots.base_value_ = base_value_;

  return knots;
}

}  // qw_units Namespace
