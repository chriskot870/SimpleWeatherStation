
#include "fahrenheit.h"

namespace qw_units {

class Celsius;
class Kelvin;

/*
 * Constructor routines
 */
Fahrenheit::Fahrenheit(){};

Fahrenheit::Fahrenheit(float temp) {

  base_value_ = FahrenheitToBase(temp);

  return;
}

Fahrenheit::Fahrenheit(float temp, string fmt_value) {

  base_value_ = FahrenheitToBase(temp);

  fmt_value_ = fmt_value;

  return;
}

/*
 * Data manipulation routines
 */
float Fahrenheit::value() {

  return BaseToFahrenheit(base_value_);
}

int Fahrenheit::FahrenheitToBase(float temp) {

  float f = (((temp - 32) * 5) / 9) * temperature_base_conversion_factor;
  int value = round(f);

  return value;
}

float Fahrenheit::BaseToFahrenheit(int base) {

  float value =
      ((((float)base / temperature_base_conversion_factor) * 9) / 5) + 32;

  return value;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Fahrenheit::toString() {

  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Fahrenheit::toString(string fmt_value) {

  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this specific instance
 */
void Fahrenheit::setFormat(string fmt_value) {

  fmt_value_ = fmt_value;

  return;
}

/*
 * Used by conversion routines for implicit cast
 */
void Fahrenheit::setBaseValue(int base_value) {
  base_value_ = base_value;

  return;
}

/*********************************
 * Operator functions
 *********************************/
/*
 * Comparison operators
 */
bool Fahrenheit::operator==(const Fahrenheit& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Fahrenheit::operator!=(const Fahrenheit& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Fahrenheit::operator<(const Fahrenheit& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;
}

bool Fahrenheit::operator>(const Fahrenheit& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Fahrenheit::operator<=(const Fahrenheit& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Fahrenheit::operator>=(const Fahrenheit& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Fahrenheit::operator<=> (const Fahrenheit& other) const {

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
Fahrenheit& Fahrenheit::operator=(const Fahrenheit& other) {

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

/*
 * Arithmetic operators
 */
Fahrenheit& Fahrenheit::operator+=(const Fahrenheit& other) {

  base_value_ += other.base_value_;

  return *this;
}

Fahrenheit& Fahrenheit::operator-=(const Fahrenheit& other) {

  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Fahrenheit Fahrenheit::operator+(const Fahrenheit& other) const {

  Fahrenheit result = *this;

  result += other;

  return result;
}

const Fahrenheit Fahrenheit::operator-(const Fahrenheit& other) const {

  Fahrenheit result = *this;

  result -= other;

  return result;
}

/*
 * Conversion operators used for implicit casting
 */
Fahrenheit::operator Celsius() const {

  Celsius tempc;

  tempc.base_value_ = base_value_;

  return tempc;
}

Fahrenheit::operator Kelvin() const {

  Kelvin tempk;

  tempk.base_value_ = base_value_;

  return tempk;
}

}  // Namespace qw_units
