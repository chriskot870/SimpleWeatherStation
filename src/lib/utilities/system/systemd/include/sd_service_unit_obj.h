#ifndef LIB_UTILITIES_SYSTEM_SYSTEMD_SD_SERVICE_UNIT_OBJ_H_
#define LIB_UTILITIES_SYSTEM_SYSTEMD_SD_SERVICE_UNIT_OBJ_H_

#include "systemd.h"

class SdServiceUnitObj {
 public:
  SdServiceUnitObj(string destination, string path, string interface);

  /*
   * Properties this application needs
   */
  expected<uint64_t, SdBusError> getMainPID();

 private:
 /*
  * local variables
  */
  string destination_;
  string path_;
  string interface_;
};

#endif  // LIB_UTILITIES_SYSTEM_SYSTEMD_SD_SERVICE_UNIT_OBJ_H_
