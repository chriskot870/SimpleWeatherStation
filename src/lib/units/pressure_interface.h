#ifndef LIB_UNITS_BAROMETRIC_PRESSURE_INTERFACE_H
#define LIB_UNITS_BAROMETRIC_PRESSURE_INTERFACE_H
/*
 * This contains the BarometricPressureInterface
 */

typedef enum {
  PRESSURE_UNIT_hPa,            // hundrerd Pascals
  PRESSURE_UNIT_Pa,             // Pascals
  PRESSURE_UNIT_PSI,            // Pounds Per Square Inch
  PRESSURE_UNIT_INCHES_MERCURY  // Inches of Mercury
} PressureUnit_t;

constexpr float PressurePascalsPerPsi = 6894.76;
constexpr int PressurePascalsPerhPa = 100;
constexpr float PressurePascalsPerInchesMercury = 3386.39;

class PressureConversion {
 public:
  static float PaTohPa(float pa) {
    float hPa;

    hPa = pa / PressurePascalsPerhPa;

    return hPa;
  }

  static float PaToPsi(float pa) {
    float psi;

    psi = pa / PressurePascalsPerPsi;

    return psi;
  }

  static float hPaToPa(float hpa) {
    float pa;

    pa = hpa * PressurePascalsPerhPa;

    return pa;
  }

  static float hPaToPsi(float hpa) {
    float psi;

    psi = PaToPsi(hPaToPa(hpa));

    return psi;
  }

  static float PsiToPa(float psi) {
    float pa;

    pa = psi * PressurePascalsPerPsi;

    return pa;
  }

  static float PsiTohPa(float psi) {
    float hpa;

    hpa = PaTohPa(PsiToPa(psi));

    return hpa;
  }

  static float PaTohInchesMercury(float pa) {
    float inches;

    inches = pa / PressurePascalsPerInchesMercury;

    return inches;
  }

  static float hPaTohInchesMercury(float hpa) {
    float inches;

    inches = PaTohInchesMercury(hPaToPa(hpa));

    return inches;
  }

  static float InchesMercuryTohPa(float inches) {
    float hpa;

    inches = PaTohPa(inches * PressurePascalsPerInchesMercury);

    return inches;
  }
};

class BarometricPressureInterface {
 public:
  /*
   * The Barometric Pressure
   */

  virtual float getBarometricPressure(PressureUnit_t unit) = 0;

  virtual ~BarometricPressureInterface() {}
};

#endif  // LIB_UNITS_BAROMETRIC_PRESSURE_INTERFACE_H
