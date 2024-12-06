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

class Celsius : public Temperature {
 public:
    Celsius();

    Celsius(float temp);

    Celsius(float temp, string fmt_value);

    /*
     * These constructors are for casting
     */
   // Celsius(const Fahrenheit& obj);

    //Celsius(const Kelvin& obj);

    /*
     * I thought I needed these for casting but found a site
     * that used the above.
     */
    operator Fahrenheit() const;

    operator Kelvin() const;

    Celsius& operator=(const Celsius& other);

    float value();

    int dummy;

};

}  // Namespace qw_units

#endif  // LIB_UNITS_TEMPERATURE_FAHRENHEIT_H_