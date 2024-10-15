#ifndef LIB_UNITS_BAROMETRIC_PRESSURE_INTERFACE_H_
#define LIB_UNITS_BAROMETRIC_PRESSURE_INTERFACE_H_

#include <expected>
/*
 * This contains the BarometricPressureInterface
 */

typedef enum {
  PRESSURE_UNIT_Mb,            // millibars which is an hPa, 100 pascals
  PRESSURE_UNIT_Pa,             // Pascals
  PRESSURE_UNIT_PSI,            // Pounds Per Square Inch
  PRESSURE_UNIT_INCHES_MERCURY  // Inches of Mercury
} PressureUnit_t;

constexpr float PressurePascalsPerPsi = 6894.76;
constexpr int PressurePascalsPerMb = 100;
constexpr float PressurePascalsPerInchesMercury = 3386.39;

class PressureConversion {
 public:
  static float PaToMb(float pa) {
    float mb;

    mb = pa / PressurePascalsPerMb;

    return mb;
  }

  static float PaToPsi(float pa) {
    float psi;

    psi = pa / PressurePascalsPerPsi;

    return psi;
  }

  static float MbToPa(float mb) {
    float pa;

    pa = mb * PressurePascalsPerMb;

    return pa;
  }

  static float MbToPsi(float mb) {
    float psi;

    psi = PaToPsi(MbToPa(mb));

    return psi;
  }

  static float PsiToPa(float psi) {
    float pa;

    pa = psi * PressurePascalsPerPsi;

    return pa;
  }

  static float PsiToMb(float psi) {
    float mb;

    mb = PaToMb(PsiToPa(psi));

    return mb;
  }

  static float PaTohInchesMercury(float pa) {
    float inches;

    inches = pa / PressurePascalsPerInchesMercury;

    return inches;
  }

  static float MbTohInchesMercury(float mb) {
    float inches;

    inches = PaTohInchesMercury(MbToPa(mb));

    return inches;
  }

  static float InchesMercuryToMb(float inches) {
    float mb;

    mb = PaToMb(inches * PressurePascalsPerInchesMercury);

    return mb;
  }
};

class BarometricPressureInterface {
 public:
  /*
   * The Barometric Pressure
   */

  virtual std::expected<float, int> getBarometricPressure(
      PressureUnit_t unit) = 0;

  virtual ~BarometricPressureInterface() {}
};

#endif  // LIB_UNITS_BAROMETRIC_PRESSURE_INTERFACE_H_
