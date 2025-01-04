/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UTILITIES_SYSTEM_LOGGER_H_
#define LIB_UTILITIES_SYSTEM_LOGGER_H_

#include <string>
#include <syslog.h>
#include <systemd/sd-journal.h>
#include <filesystem>
#include <iostream>
#include <fstream>

using std::string;

enum LoggerMode {
  LOGGER_MODE_NOLOGGING,
  LOGGER_MODE_FILE,
  LOGGER_MODE_JOURNAL,
};

class Logger {
 public:
  Logger();

  void log(int priority, string message);

  void setMode(LoggerMode mode);

  void setMode(LoggerMode mode, std::filesystem::path log_path);

  LoggerMode getMode();

  ~Logger();

 private:
  LoggerMode mode_ = LOGGER_MODE_NOLOGGING;

  std::ofstream log_stream_;

  std::streambuf *cout_buffer_ = nullptr;

  std::filesystem::path log_path_ = "";

};

#endif  // LIB_UTILITIES_SYSTEM_LOGGER_H_