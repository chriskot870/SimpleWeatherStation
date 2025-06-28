
#ifndef LIB_UTILITIES_SYSTEM_SYSTEMD_SD_MANAGER_OBJ_H_
#define LIB_UTILITIES_SYSTEM_SYSTEMD_SD_MANAGER_OBJ_H_

#include "systemd.h"

class SdManager {
 public:
  SdManager(string destination, string path, string interface);

  expected<string, SdBusError> StartUnit(string name, string mode);

  expected<string, SdBusError> StopUnit(string name, string mode);
  
 private:
  /*
   * local variables
   */
  string destination_;
  string path_;
  string interface_;
};

#endif  // LIB_UTILITIES_SYSTEM_SYSTEMD_SD_MANAGER_OBJ_H_
