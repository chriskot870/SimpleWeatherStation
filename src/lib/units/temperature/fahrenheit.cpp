

#include "fahrenheit.h"

namespace qw_units {

Fahrenheit::Fahrenheit(){};

Fahrenheit::Fahrenheit(float temp){

  base_value_ = FahrenheitToBase(temp);

  return;
}

Fahrenheit::Fahrenheit(float temp, string fmt_value) {

  base_value_ = FahrenheitToBase(temp);

  fmt_value_ = fmt_value;

  return;
}

float Fahrenheit::value() {

    return BaseToFahrenheit(base_value_);
}

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

}  // Namespace qw_units

