/*
 * Copyright 2024 Chris Kottaridis
 */

#include "pressure_datum.h"

PressureDatum::PressureDatum (float value,  PressureUnit_t unit) : value_(value), unit_(unit) {}

/*
 * Static helper functions to convert a value from one unit to another
 */
float PressureDatum::PaToMb(float pa) {
  float mb;

  mb = pa / PressurePascalsPerMb;

  return mb;
}

float PressureDatum::PaToPsi(float pa) {
  float psi;

  psi = pa / PressurePascalsPerPsi;

  return psi;
}

float PressureDatum::MbToPa(float mb) {
  float pa;

  pa = mb * PressurePascalsPerMb;

  return pa;
}

float PressureDatum::MbToPsi(float mb) {
  float psi;

  psi = PaToPsi(MbToPa(mb));

  return psi;
  }

float PressureDatum::PsiToPa(float psi) {
  float pa;

  pa = psi * PressurePascalsPerPsi;

  return pa;
}

float PressureDatum::PsiToMb(float psi) {
  float mb, pa;

  mb = PressureDatum::PaToMb(PressureDatum::PsiToPa(psi));

  return mb;
}

float PressureDatum::PaToInchesMercury(float pa) {
  float inches;

  inches = pa / PressurePascalsPerInchesMercury;

  return inches;
}

float PressureDatum::MbToInchesMercury(float mb) {
  float inches;

  inches = PressureDatum::PaToInchesMercury(PressureDatum::MbToPa(mb));

  return inches;
}

float PressureDatum::InchesMercuryToMb(float inches) {
  float mb;

  mb = PressureDatum::PaToMb(inches * PressurePascalsPerInchesMercury);

  return mb;
}

/*
 * Non-static Functions that return the private values
 */
float PressureDatum::getValue() {
  return value_;
}

PressureUnit_t PressureDatum::getUnit() {
  return unit_;
}
