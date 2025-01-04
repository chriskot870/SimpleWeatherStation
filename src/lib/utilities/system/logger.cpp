#include "logger.h"

Logger logger;

using std::string;

Logger::Logger() {}

void Logger::log(int priority, string message) {

switch(mode_) {
  case LOGGER_MODE_NOLOGGING :
    break;
  case LOGGER_MODE_FILE :
    std::cout << message << std::endl;
    std::cout.flush();
    break;
  case LOGGER_MODE_JOURNAL :
    sd_journal_print(priority, message.c_str());
    break;
  }

  return;
}

void Logger::setMode(LoggerMode mode) {

  mode_ = mode;

  if (mode == LOGGER_MODE_FILE) {
    if (log_path_ == "");
    if ((log_path_ != "") && (log_stream_.is_open() == true)) {
      if (std::cout.rdbuf() == log_stream_.rdbuf()) {
        /*
         * Set the std::cout.rdbuf() back to std::cout's buffer
         */
        std::cout.rdbuf(cout_buffer_);
      }
      log_stream_.close();
    }
    log_path_ = "";
  }

  return;
}

void Logger::setMode(LoggerMode mode, std::filesystem::path log_path) {

  mode_ = mode;
  
  /*
   * If the current log file is the same as the new one just exit
   */
  if (log_path == log_path_) {
    return;
  }

  log_path_ = log_path;

  /*
   * Check if the current cout.rdbuf() is set to log_stream_
   */
  if (std::cout.rdbuf() == log_stream_.rdbuf()) {
    /*
     * Set the std::cout.rdbuf() back to std::cout's buffer
     */
    std::cout.rdbuf(cout_buffer_);
  }

  /*
   * If the current file is open close it.
   */
  if (log_stream_.is_open() == true) {
    log_stream_.close();
  }

  /*
   * Assign log_stream_ to the new file
   */
  log_stream_ = std::ofstream(log_path.string(), std::ios::out | std::ios::app);
  /*
   * Assign the cout.rdbuf() to the new file's rdbuf()
   */
  cout_buffer_ = std::cout.rdbuf();
  std::cout.rdbuf(log_stream_.rdbuf());

  return;
}

LoggerMode Logger::getMode() {

  return mode_;
}

Logger::~Logger() {

  /*
   * If the log_strem_.rdbuf() is the same as std::cout.rdbuf()
   * Put the old cout_buffer_ back to std::cout.
   */
  if (log_stream_.rdbuf() == std::cout.rdbuf()) {
    std::cout.rdbuf(cout_buffer_);
    cout_buffer_ = nullptr;
  }

return;
}

