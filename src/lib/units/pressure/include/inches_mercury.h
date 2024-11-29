/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_PRESSURE_INCHES_MERCURY_H_
#define LIB_UNITS_PRESSURE_INCHES_MERCURY_H_

#include "pressure.h"
#include "millibar.h"

namespace qw_units {
/*
 * Need to predeclare these classes
 * I found that out the hard way
 */
class Millibar;

class InchesMercury : private Pressure {
 public:
    InchesMercury();

    InchesMercury(float temp);

    /*
     * Supports implicit casting
     * hence the need for the predeclaration
     */
    operator Millibar() const;

    float getValue();

};
}
#endif  // LIB_UNITS_PRESSURE_INCHES_MERCURY_H_