/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UTILITIES_SYSTEM_SD_BUS_H_
#define LIB_UTILITIES_SYSTEM_SD_BUS_H_

#include <string>
#include <map>
#include <systemd/sd-bus.h>

using std::string;

class SdBusService {
 public:

  SdBusService(string name);

  string name_;
};

class SdBusObject {
 public:

  SdBusObject(string name, const SdBusService& sdbus_service);

  string name_;
  SdBusService sdbus_service_;
};

class SdBusInterface {
 public:

  SdBusInterface(string name, const SdBusObject& sdbus_object);

  string name_;
  SdBusObject sdbus_object_;
};

class SdBusMethod {

 public:

  SdBusMethod(string name, string signature, string result_value, string flags, const SdBusInterface& sdbus_interface);

  string name_;
  string signature_;
  string flags_;
  SdBusInterface sdbus_interface_;
};

class SdBusProperty {
 public:

  SdBusProperty(string name, string signature, string flags, const SdBusInterface& sdbus_interface);

  int getProperty(sd_bus_message **message, sd_bus_error *error);

 public:
  string name_;
  string signature_;
  string flags_;
  SdBusInterface sdbus_interface_;
};

#endif  // LIB_UTILITIES_SYSTEM_SD_BUS_H_
