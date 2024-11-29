 /*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_TEMPERATURE_CELSIUS_H_
#define LIB_UNITS_TEMPERATURE_CELSIUS_H_

#include "temperature.h"
#include "fahrenheit.h"
#include "kelvin.h"

namespace qw_units {
/*
 * Need to predeclare these classes
 * I found that out the hard way
 */
class Fahrenheit;
class Kelvin;

class Celsius : private Temperature {
 public:
    Celsius();

    Celsius(float temp);

    /*
     * These constructors are for casting
     */
   // Celsius(const Fahrenheit& obj);

    //Celsius(const Kelvin& obj);

    /*
     * I thought I needed these for casting but found a site
     * that used the above.
     */
    operator Fahrenheit();

    operator Kelvin();

    Celsius& operator=(const Celsius& other);

    float getValue();

    int dummy;

};

}

#endif  // LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_