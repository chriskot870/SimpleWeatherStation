
#include "celsius.h"

namespace qw_units {

class Kelvin;
class Fahrenheit;

Celsius::Celsius() {}

Celsius::Celsius(float temp){

  base_value_ = CelsiusToBase(temp);

  return;
}

Celsius::Celsius(float temp, string fmt_value) {

  base_value_ = CelsiusToBase(temp);

  fmt_value_ = fmt_value;

  return;
}

/*
 * data manipulation routines
 */
float Celsius::value() {

    return BaseToCelsius(base_value_);
}

int Celsius::CelsiusToBase(float temp) {

  float f = temp * temperature_base_conversion_factor;
  int value = round(f);

  return value;
}

float Celsius::BaseToCelsius(int base) {

  float temp = (float)base / temperature_base_conversion_factor;

  return temp;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Celsius::toString() {

  string data = format(fmt::runtime(fmt_value_), base_value_);

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Celsius::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), base_value_);

  return data;
}

void Celsius::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

void Celsius::setBaseValue(int base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Comparison operators
 */
bool Celsius::operator==(const Celsius& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Celsius::operator!=(const Celsius& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Celsius::operator<(const Celsius& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;

}

bool Celsius::operator>(const Celsius& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Celsius::operator<=(const Celsius& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Celsius::operator>=(const Celsius& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Celsius::operator<=>(const Celsius& other) const {

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
Celsius& Celsius::operator=(const Celsius& other) {

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

Celsius& Celsius::operator+=(const Celsius& other) {

  base_value_ += other.base_value_;

  return *this;
}

Celsius& Celsius::operator-=(const Celsius& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Celsius Celsius::operator+(const Celsius &other) const {

  Celsius result = *this;

  result += other;

  return result;
}

const Celsius Celsius::operator-(const Celsius &other) const {

  Celsius result = *this;

  result -= other;

  return result;
}

/*
 * Conversion routines for implicit casting to friends
 */
Celsius::operator Kelvin() const {

  Kelvin tempk;

  tempk.setBaseValue(base_value_);

  return tempk;
}

Celsius::operator Fahrenheit() const {
    
  Fahrenheit tempf;

  tempf.setBaseValue(base_value_);

  return tempf;
}

}  // Namespace qw_units

