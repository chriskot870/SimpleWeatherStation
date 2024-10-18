/*
 * Copyright 2024 Chris Kottaridis
 */

#include "relative_humidity_datum.h"

RelativeHumidityDatum::RelativeHumidityDatum (float value,  RelativeHumidityUnit_t unit) : value_(value), unit_(unit) {}
/*
 * Public functions
 */

/*
 * Return the value of the datum
 */
float RelativeHumidityDatum::getValue() {
    return(value_);
}

/*
 * Return the unit of the datum
 */
RelativeHumidityUnit_t RelativeHumidityDatum::getUnit() {
    return(unit_);
}
