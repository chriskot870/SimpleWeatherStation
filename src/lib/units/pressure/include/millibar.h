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

class Millibar : public Pressure {
 public:
    Millibar();

    Millibar(float temp);

    Millibar(float temp, string fmt_value);

    /*
     * Supports implicit casting
     */
    operator InchesMercury() const;

    float value();

};

}  // Namespace qw_units

#endif  // LIB_UNITS_PRESSURE_MILLIBAR_H_