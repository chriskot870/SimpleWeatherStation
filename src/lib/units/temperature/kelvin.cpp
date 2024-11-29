#include "kelvin.h"

namespace qw_units {

Kelvin::Kelvin(){};

Kelvin::Kelvin(float temp){

  base_value_ = KelvinToBase(temp);

  return;
}

Kelvin::operator Celsius() const {

  return Celsius(base_value_);
}

Kelvin::operator Fahrenheit() const {
    
  return Fahrenheit(base_value_);
}

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

float Kelvin::getValue() {

  return BaseToKelvin(base_value_);
}

}
