/*
 * Copyright 2024 Chris Kottaridis
 */

#include "temperature_datum.h"

TemperatureDatum::TemperatureDatum (float value,  TemperatureUnit_t unit) : value_(value), unit_(unit) {

  switch (unit) {
    case   TEMPERATURE_UNIT_FAHRENHEIT :
      base_value_ = round(fahrenheitToCelsius(value_) * 1000);
      break;
    case TEMPERATURE_UNIT_CELSIUS :
      base_value_ = round(value_ * 1000);
      break;
    case TEMPERATURE_UNIT_KELVIN :
      base_value_ = round(celsiusToKelvin(value_) * 1000);
      break;
  }
}

  /*
   * Static conversion functions
   */
  float TemperatureDatum::celsiusToFahrenheit(float celsius) {
    float fahrenheit;

    fahrenheit = ((celsius * 9) / 5) + 32;

    return fahrenheit;
  }

  float TemperatureDatum::celsiusToKelvin(float celsius) {
    float kelvin;

    kelvin = celsius + 273.15;

    return kelvin;
  }

  float TemperatureDatum::fahrenheitToCelsius(float fahrenheit) {
    float celsius;

    celsius = ((fahrenheit - 32) * 5) / 9;

    return celsius;
  }

  float TemperatureDatum::fahrenheitToKelvin(float fahrenheit) {
    float kelvin;

    kelvin = celsiusToKelvin(fahrenheitToCelsius(fahrenheit));

    return kelvin;
  }

  float TemperatureDatum::kelvinToCelsius(float kelvin) {
    float celsius;

    celsius = kelvin - 273.15;

    return celsius;
  }

  float TemperatureDatum::kelvinToFahrenheit(float kelvin) {
    float fahrenheit;

    fahrenheit = celsiusToFahrenheit(kelvinToCelsius(kelvin));

    return fahrenheit;
  }

/*
 * Public functions
 */

/*
 * Return the value of the datum
 */
float TemperatureDatum::getValue() {
    return(value_);
}

/*
 * Return the unit of the datum
 */
TemperatureUnit_t TemperatureDatum::getUnit() {
    return(unit_);
}
