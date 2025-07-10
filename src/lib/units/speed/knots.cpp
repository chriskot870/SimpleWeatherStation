#include "knots.h"

namespace qw_units {

  class MilesPerHour;
  class KilometersPerHour;
  class MetersPerSecond;

/*
 * Constructor routines
 */
Knots::Knots() {}

Knots::Knots(float knt) : base_value_(KnotsToBase(knt)) {}

Knots::Knots(float knt, string fmt_value) :
  base_value_(KnotsToBase(knt)), fmt_value_(fmt_value) {}

Knots::Knots(int64_t base_value) : base_value_(base_value) {}

/*
 * Data manipulation routines
 */
float Knots::value() {

  return BaseToKnots(base_value_);
}

/*
 * For every mile per hour there are 100 base units.
 * So convert knots to miles per hour. Then convert to base units.
 */
int Knots::KnotsToBase(float knots) {

  int value = round(knots * kMphPerKnot * speed_base_conversion_factor);

  return value;
}

/*
 * Convert the base value to miles per hour.
 * Then convert miles per hour to knots
 */
float Knots::BaseToKnots(int base) {

  float knots = ((float)base / speed_base_conversion_factor) * kKnotPerMph;

  return knots;
}

/*
 * Use the default format
 */
string Knots::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format instead of one in private variable
 */
string Knots::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this instance
 */
void Knots::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

/*
 * Used by conversion routines for implicit casting
 */
void Knots::setBaseValue(int base_value) {
  
  base_value_ = base_value;

  return;
}

bool Knots::operator==(const Knots& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Knots::operator!=(const  Knots& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Knots::operator<(const Knots& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool Knots::operator>(const Knots& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Knots::operator<=(const Knots& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Knots::operator>=(const Knots& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Knots::operator<=> (const Knots& other) const {

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
Knots& Knots::operator=(const Knots& other) {

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

Knots& Knots::operator+=(const Knots& other) {

  base_value_ += other.base_value_;

  return *this;
}

Knots& Knots::operator-=(const Knots& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Knots Knots::operator+(const Knots& other) const {

  Knots result = *this;

  result += other;

  return result;
}

const Knots Knots::operator-(const Knots& other) const {

  Knots result = *this;

  result -= other;

  return result;
}

/*
 * Intrinsic casting to other speed units
 */
Knots::operator KilometersPerHour() const {

  KilometersPerHour kph(base_value_);

  return kph;
}

Knots::operator MetersPerSecond() const {

  MetersPerSecond mps(base_value_);

  return mps;
}

Knots::operator MilesPerHour() const {

  MilesPerHour mph(base_value_);

  return mph;
}

}  // qw_units Namespace
