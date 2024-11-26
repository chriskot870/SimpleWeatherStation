/*
 * This file contains free routines to dteremine the Dew Point
 */

#include <dew_point.h>

expected<TemperatureDatum, int> getDewPoint(TemperatureDatum temperature, RelativeHumidityDatum humidity, TemperatureUnit_t unit) {
  float temp_value;
  float compute_temp;

  /*
   * Formula is Td = T - ((100 - RH)/5)
   * This formula is for Celsius. So, have to get Celsius value
   */
  switch (temperature.getUnit()) {
    case TEMPERATURE_UNIT_FAHRENHEIT :
      compute_temp = TemperatureDatum::fahrenheitToCelsius(temperature.getValue());
      break;
    case TEMPERATURE_UNIT_CELSIUS :
      compute_temp = temperature.getValue();
      break;
    case TEMPERATURE_UNIT_KELVIN :
      compute_temp = TemperatureDatum::kelvinToCelsius(temperature.getValue());
  }

  temp_value = compute_temp - ((100 - humidity.getValue())/5);

  /*
   * temp_value is in Celsius. Check what value was desired and convert to it
   */
  switch (unit) {
    case TEMPERATURE_UNIT_FAHRENHEIT :
      temp_value = TemperatureDatum::celsiusToFahrenheit(temp_value);
      break;
    case TEMPERATURE_UNIT_CELSIUS :
      break;
    case TEMPERATURE_UNIT_KELVIN :
      temp_value = TemperatureDatum::celsiusToKelvin(temp_value);
  }

  return TemperatureDatum(temp_value, unit);

}

expected<TemperatureMeasurement, int> getDewPointMeasurement( TemperatureMeasurement temperature, RelativeHumidityMeasurement humidity) {
  /*
   * Since this is a measurement we expect both measurements to have been made at about the same time.
   * So, we want to make sure they are within dew_point_measurement_time_spread of each other
   */
  if (duration_cast<minutes>(temperature.getTime() - humidity.getTime()) > dew_point_measurement_time_spread) {
    return unexpected(ERANGE);
  }

  expected<TemperatureDatum, int> dew_point_temp = getDewPoint(temperature.getDatum(), humidity.getDatum(), temperature.getDatum().getUnit());
  if (dew_point_temp.has_value()) {
    return unexpected(dew_point_temp.error());
  }

  TemperatureMeasurement dew_point_msmnt(dew_point_temp.value(), temperature.getTime());

  return dew_point_msmnt;
}

