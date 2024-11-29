/*
 *
 */
#include "temperature.h"

namespace qw_units {

Temperature::Temperature(){}

Temperature::Temperature(int base_value) : base_value_(base_value) {}

bool Temperature::operator==(const Temperature& other) const {

  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Temperature::operator!=(const Temperature& other) const {

  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Temperature::operator<(const Temperature& other) const {

  bool value = (base_value_ < other.base_value_);

  return value;

}

bool Temperature::operator>(const Temperature& other) const {

  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Temperature::operator<=(const Temperature& other) const {

  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Temperature::operator>=(const Temperature& other) const {

  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Temperature::operator<=>(const Temperature& other) const {

  /*
   * The <=> returns a std::strong_ordering type.
   * Either ::less, ::equal, or ::greater
   */
  strong_ordering value = (base_value_ <=> other.base_value_);

  return value;
}


Temperature& Temperature::operator+=(const Temperature& other) {

  base_value_ += other.base_value_;

  return *this;
}

Temperature& Temperature::operator-=(const Temperature& other) {

  base_value_ -= other.base_value_;

  return *this;
}
   
/*
 * I am using a base of millicelsius
 */
int Temperature::CelsiusToBase(float temp) {

  int value = round(temp * temperature_base_conversion_factor);

  return value;
}

float Temperature::BaseToCelsius(int base) {

  float temp = base / temperature_base_conversion_factor;

  return temp;
}

int Temperature::FahrenheitToBase(float temp) {

  int  value = round((((temp - 32)*5)/9) * temperature_base_conversion_factor);

  return value;
}

float Temperature::BaseToFahrenheit(int base) {

  float value = ((((float)base/temperature_base_conversion_factor)*9)/5) + 32;

  return value;
}

int Temperature::KelvinToBase(float temp) {

  int value = round((temp - temperature_celsius_kelvin_offset) * temperature_base_conversion_factor);

  return value;
}

float Temperature::BaseToKelvin(int base) {

  float value = (base + temperature_celsius_kelvin_offset)/temperature_base_conversion_factor;

  return value;
}

}
