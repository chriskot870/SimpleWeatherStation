#include "speed.h"
#include "miles_per_hour.h"
#include "kilometers_per_hour.h"
#include "meters_per_second.h"

namespace qw_units {
class Knots {

  /*
   * Need to predeclare these classes
   * I found that out the hard way
   */
  class MilesPerHour;
  class KilometersPerHour;
  class MetersPerSecond;

 public:

  friend MilesPerHour;
  friend KilometersPerHour;
  friend MetersPerSecond;

  Knots();

  Knots(float temp);

  Knots(float temp, string fmt_value);

  float value();

  string toString();

  string toString(string format);

  void setFormat(string fmt_value);

  bool operator==(const Knots& other) const;

  bool operator!=(const Knots& other) const;

  bool operator<(const Knots& other) const;

  bool operator>(const Knots& other) const;

  bool operator<=(const Knots& other) const;

  bool operator>=(const Knots& other) const;

  strong_ordering operator<=> (const Knots& other) const;

  Knots& operator=(const Knots& other);

  Knots& operator+=(const Knots& other);

  Knots& operator-=(const Knots& other);

  const Knots operator+(const Knots& other) const;

  const Knots operator-(const Knots& other) const;


 private:
  int64_t base_value_;

  string fmt_value_ = speed_default_format;

  int KnotsToBase(float temp);

  float BaseToKnots(int base);

  void setBaseValue(int base_value);
};

}  //qw_units Namespace