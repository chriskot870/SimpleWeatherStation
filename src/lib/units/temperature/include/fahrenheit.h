/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_
#define LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_

#include "temperature.h"
#include "celsius.h"
#include "kelvin.h"

namespace qw_units {
/*
 * Need to predeclare these classes
 * I found that out the hard way
 */

class Celsius;
class Kelvin;

class Fahrenheit : public Temperature {
 public:
    Fahrenheit();

    Fahrenheit(float temp);

    operator Celsius() const;

    operator Kelvin() const;

    Fahrenheit& operator=(const Fahrenheit& other);

    int getValue();

};

}

#endif  // LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_