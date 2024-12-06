 /*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_KELVIN_H_
#define LIB_UNITS_TEMPERATURE_KELVIN_H_

#include "temperature.h"
#include "celsius.h"
#include "fahrenheit.h"

namespace qw_units {
/*
 * Need to predeclare these classes
 * I found this out the hard way
 */
class Celsius;
class Fahrenheit;

class Kelvin : public Temperature {
 public:
    Kelvin();

    Kelvin(float temp);

    Kelvin(float temp, string fmt_value);

    operator Celsius() const;

    operator Fahrenheit() const;

    Kelvin& operator=(const Kelvin& other);

    float value();

};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_KELVIN_H_