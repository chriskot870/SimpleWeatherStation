#include "kelvin.h"

namespace qw_units {

class Celsius;
class Fahrenheit;

Kelvin::Kelvin(){};

Kelvin::Kelvin(float temp){

  base_value_ = KelvinToBase(temp);

  return;
}

Kelvin::Kelvin(float temp, string fmt_value) {

  base_value_ = KelvinToBase(temp);

  fmt_value_ = fmt_value;

  return;
}

/*
 * Data manipulation routines
 */
float Kelvin::value() {

  return BaseToKelvin(base_value_);
}

int Kelvin::KelvinToBase(float temp) {

  float f = ((temp - temperature_celsius_kelvin_offset) * temperature_base_conversion_factor);
  int value = round(f);

  return value;
}

float Kelvin::BaseToKelvin(int base) {

  float value = ((float)base + temperature_celsius_kelvin_offset)/temperature_base_conversion_factor;

  return value;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Kelvin::toString() {

  string data = format(fmt::runtime(fmt_value_), base_value_);

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Kelvin::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), base_value_);

  return data;
}

void Kelvin::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

void Kelvin::setBaseValue(int base_value) {
  base_value_ = base_value;

  return;
}

/****************************
 * Operator routines
 ****************************/
/*
 * Comparison operators
 */
bool Kelvin::operator==(const Kelvin& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Kelvin::operator!=(const Kelvin& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Kelvin::operator<(const Kelvin& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;

}

bool Kelvin::operator>(const Kelvin& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Kelvin::operator<=(const Kelvin& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Kelvin::operator>=(const Kelvin& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Kelvin::operator<=>(const Kelvin& other) const {

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
Kelvin& Kelvin::operator=(const Kelvin& other) {

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

Kelvin& Kelvin::operator+=(const Kelvin& other) {

  base_value_ += other.base_value_;

  return *this;
}

Kelvin& Kelvin::operator-=(const Kelvin& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Kelvin Kelvin::operator+(const Kelvin &other) const {

  Kelvin result = *this;

  result += other;

  return result;
}

const Kelvin Kelvin::operator-(const Kelvin &other) const {

  Kelvin result = *this;

  result -= other;

  return result;
}

/*
 * Conversion operator used for implicit casts
 */
Kelvin::operator Celsius() const {

  Celsius tempc;

  tempc.setBaseValue(base_value_);

  return tempc;
}

Kelvin::operator Fahrenheit() const {

  Fahrenheit tempf;

  tempf.setBaseValue(base_value_);
    
  return tempf;
}

}  // Namespace qw_units
