/*
 *
 */

#ifndef LIB_UNITS_DEW_POINT_H_
#define LIB_UNITS_DEW_POINT_H_

#include <chrono>
#include <expected>

#include "temperature_datum.h"
#include "temperature_measurement.h"
#include "relative_humidity_datum.h"
#include "relative_humidity_measurement.h"

using std::expected;
using std::unexpected;
using std::chrono::minutes;

constexpr minutes dew_point_measurement_time_spread = minutes(5);
/*
 * The dew point is the temperature at which the air is saturated.
 * The unit value of the returned value will be same as the temperature Datum provided
 */
expected<TemperatureDatum, int> getDewPoint(TemperatureDatum temperature, RelativeHumidityDatum humidity, TemperatureUnit_t unit);

/*
 * The dew point measurement returns a Temperature measurment value. The time value
 * will be the same as the time of the temperature. Since both input measurements have
 * a time value it implies that the measurment are at the same time. A check is made
 * to verify that the measurement are withn a certin time of each other. If they are
 * too far a part then an error is returned.
 */
expected<TemperatureMeasurement, int> getDewPointMeasurement( TemperatureMeasurement temperature, RelativeHumidityMeasurement humidity);

#endif // LIB_UNITS_DEW_POINT_H_