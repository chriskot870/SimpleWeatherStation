/*
 * This contains the abstract class for temperature
 */
#ifndef LIB_UNITS_TEMPERATURE_H_
#define LIB_UNITS_TEMPERATURE_H_

typedef enum {
  TEMPERATURE_UNIT_FAHRENHEIT,
  TEMPERATURE_UNIT_CELSIUS,
  TEMPERATURE_UNIT_KELVIN
} TemperatureUnit_t;

/*
 * These are helper static routines to convert temperatures
 */
class TemperatureConversion {
 public:
  static float celsiusToFahrenheit(float celsius) {
    float fahrenheit;

    fahrenheit = ((celsius * 9) / 5) + 32;

    return fahrenheit;
  }

  static float celsiusToKelvin(float celsius) {
    float kelvin;

    kelvin = celsius + 273.15;

    return kelvin;
  }

  static float fahrenheitToCelsius(float fahrenheit) {
    float celsius;

    celsius = ((fahrenheit - 32) * 5) / 9;

    return celsius;
  }

  static float fahrenheitToKelvin(float fahrenheit) {
    float kelvin;

    kelvin = celsiusToKelvin(fahrenheitToCelsius(fahrenheit));

    return kelvin;
  }

  static float kelvinToCelsius(float kelvin) {
    float celsius;

    celsius = kelvin - 273.15;

    return celsius;
  }

  static float kelvinToFahrenheit(float kelvin) {
    float fahrenheit;

    fahrenheit = celsiusToFahrenheit(kelvinToCelsius(kelvin));

    return fahrenheit;
  }
};

class TemperatureInterface {
 public:
  /*
   * The Temperature
   */

  virtual std::expected<float, int> getTemperature(
      TemperatureUnit_t temperature_unit) = 0;

  virtual ~TemperatureInterface() {}
};

#endif  // LIB_UNITS_TEMPERATURE_H_
