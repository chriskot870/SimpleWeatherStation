 /*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_PRESSURE_MILLIBAR_H_
#define LIB_UNITS_PRESSURE_MILLIBAR_H_

#include "pressure.h"
#include "inches_mercury.h"

namespace qw_units {
/*
 * Need to predeclare these classes
 * I found that out the hard way
 */
class InchesMercury;

class Millibar : private Pressure {
 public:
    Millibar();

    Millibar(float temp);

    /*
     * Supports implicit casting
     */
    operator InchesMercury() const;

    float getValue();

};
}
#endif  // LIB_UNITS_PRESSURE_MILLIBAR_H_