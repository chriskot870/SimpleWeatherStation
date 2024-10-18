/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UNITS_PRESSURE_DATUM_H_
#define LIB_UNITS_PRESSURE_DATUM_H_

typedef enum {
  PRESSURE_UNIT_Mb,             // millibars which is an hPa, 100 pascals
  PRESSURE_UNIT_Pa,             // Pascals
  PRESSURE_UNIT_PSI,            // Pounds Per Square Inch
  PRESSURE_UNIT_INCHES_MERCURY  // Inches of Mercury
} PressureUnit_t;

constexpr float PressurePascalsPerPsi = 6894.76;
constexpr int PressurePascalsPerMb = 100;
constexpr float PressurePascalsPerInchesMercury = 3386.39;

class PressureDatum {
 public:

  PressureDatum (float value,  PressureUnit_t unit);

  /*
   * Power Conversion  routines
   */
  static float PaToMb(float pa);

  static float PaToPsi(float pa);

  static float MbToPa(float mb);

  static float MbToPsi(float mb);

  static float PsiToPa(float psi);

  static float PsiToMb(float psi);

  static float PaToInchesMercury(float pa);

  static float MbToInchesMercury(float mb);

  static float InchesMercuryToMb(float inches);

  /*
   * Public Functions
   */
  float getValue();

  PressureUnit_t getUnit();

 private:
  float value_;
  PressureUnit_t unit_;
};


#endif  // LIB_UNITS_PRESSURE_DATUM_H_