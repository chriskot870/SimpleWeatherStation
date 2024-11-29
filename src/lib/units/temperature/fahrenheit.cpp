

#include "fahrenheit.h"

namespace qw_units {

Fahrenheit::Fahrenheit(){};

Fahrenheit::Fahrenheit(float temp){

  base_value_ = FahrenheitToBase(temp);

  return;
}

int Fahrenheit::getValue() {

    return BaseToFahrenheit(base_value_);
}

Fahrenheit::operator Celsius() const {

  return Celsius(base_value_);
}

Fahrenheit::operator Kelvin() const {
    
  return Kelvin(base_value_);
}

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

}
