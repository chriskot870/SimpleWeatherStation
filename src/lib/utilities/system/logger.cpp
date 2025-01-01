#include "logger.h"

Logger logger;

Logger::Logger() {}

void Logger::log(int priority, string message) {

  printf("%s", message.c_str());
  printf("/n");

  return;
}

