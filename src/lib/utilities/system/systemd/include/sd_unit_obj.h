
#ifndef LIB_UTILITIES_SYSTEM_SYSTEMD_SD_UNIT_OBJ_H_
#define LIB_UTILITIES_SYSTEM_SYSTEMD_SD_UNIT_OBJ_H_

#include "systemd.h"

class SdUnitObj {
 public:
  SdUnitObj(string destination, string path, string interface);
  /*
   * Methods this application can use
   */
  expected<string, SdBusError> Start(string mode);

  expected<string, SdBusError> Stop(string mode);

  /*
   * Properties this application needs
   */
  expected<string,  SdBusError> getSubState();

 private:
 /*
  * local variables
  */
  string destination_;
  string path_;
  string interface_;
};

#endif  // #ifndef LIB_UTILITIES_SYSTEM_SYSTEMD_SD_UNIT_OBJ_H_
