#include "logger.h"

Logger logger;

Logger::Logger() {}

void Logger::log(int priority, string message) {

switch(mode_) {
  case LOGGER_MODE_NOLOGGING :
    break;
  case LOGGER_MODE_PRINTF :
    printf("%s", message.c_str());
    printf("\n");
    break;
  case LOGGER_MODE_JOURNAL :
    sd_journal_print(priority, message.c_str());
    break;
  }

  return;
}

void Logger::setMode(LoggerMode mode) {

  mode_ = mode;

  return;
}

LoggerMode Logger::getMode() {

  return mode_;
}

