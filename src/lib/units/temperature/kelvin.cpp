#include "kelvin.h"

namespace qw_units {

Kelvin::Kelvin(){};

Kelvin::Kelvin(float temp){

  base_value_ = KelvinToBase(temp);

  return;
}

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

float Kelvin::value() {

  return BaseToKelvin(base_value_);
}

}
