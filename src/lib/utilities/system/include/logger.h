/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UTILITIES_SYSTEM_LOGGER_H_
#define LIB_UTILITIES_SYSTEM_LOGGER_H_

#include <string>
#include <syslog.h>
#include <systemd/sd-journal.h>

using std::string;

enum LoggerMode {
  LOGGER_MODE_NOLOGGING,
  LOGGER_MODE_PRINTF,
  LOGGER_MODE_JOURNAL,
};

class Logger {
 public:
  Logger();

  void log(int priority, string message);

  void setMode(LoggerMode mode);

  LoggerMode getMode();

 private:
  LoggerMode mode_ = LOGGER_MODE_PRINTF;

};

#endif  // LIB_UTILITIES_SYSTEM_LOGGER_H_