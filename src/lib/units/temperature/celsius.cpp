
#include "celsius.h"

namespace qw_units {

Celsius::Celsius() {}

Celsius::Celsius(float temp){

  base_value_ = CelsiusToBase(temp);

  return;
}

/*
Celsius::Celsius(const Fahrenheit& obj) : dummy(obj.base_value_) {}

Celsius::Celsius(const Kelvin& obj) : base_value_(obj.base_value_) {}
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

float Celsius::getValue() {

    return BaseToCelsius(base_value_);
}

Celsius::operator Kelvin() {

  return Kelvin(base_value_);
}

Celsius::operator Fahrenheit() {
    
  return Fahrenheit(base_value_);
}

}
