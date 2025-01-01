/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UTILITIES_SYSTEM_LOGGER_H_
#define LIB_UTILITIES_SYSTEM_LOGGER_H_

#include <string>
#include <syslog.h>
#include <systemd/sd-journal.h>

using std::string;

class Logger {
 public:
  Logger();

  void log(int priority, string message);

};

#endif  // LIB_UTILITIES_SYSTEM_LOGGER_H_