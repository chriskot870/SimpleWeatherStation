/*
 * Copyright 2024,2025, Chris Kottaridis. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * NTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "include/weather_station_config.h"

#include <string>

#include "fmt/format.h"
#include "jsoncpp/json/json.h"

#include "qw/logger/include/logger.h"

using qw::locking::LockingFile;
using qw::logging::Logger;
using qw::logging::logger;
using std::ifstream;
using std::ofstream;
using std::string;
using std::unique_ptr;
using fmt::format;
using Json::parseFromStream;
using Json::CharReaderBuilder;
using Json::StreamWriterBuilder;
using Json::StreamWriter;

WeatherStationConfig::WeatherStationConfig(const string& config_file)
    : config_file_(config_file) {}

void WeatherStationConfig::setConfigFile(const string& config_file) {
  config_file_ = config_file;

  return;
}

bool WeatherStationConfig::exists() {
  std::filesystem::path fpath = config_file_;
  if (std::filesystem::exists(fpath) == false) {
    return false;
  }

  return true;
}

bool WeatherStationConfig::getRoot(Json::Value *root) {
  CharReaderBuilder builder;
  string errors;
  string lock_file = getLockFileName(config_file_);

  LockingFile config_guard(lock_file);

  config_guard.lock();
  ifstream config_file_stream(config_file_);
  if (config_file_stream.is_open() == false) {
    logger.log(LOG_ERR, "Failed to open the WU config file.");
    exit(1);
  }

  if (parseFromStream(builder, config_file_stream, root, &errors) == false) {
    logger.log(LOG_ERR,
               format("Failed to parse config Weather Underground config file: {}", errors));
    return false;
  }
  config_file_stream.close();
  config_guard.unlock();

  return true;
}

bool WeatherStationConfig::putRoot(const Json::Value& data) {
  string lock_file = getLockFileName(config_file_);
  LockingFile config_guard(lock_file);
  config_guard.lock();

  /*
   * If we get the lock file it is then OK to access the config file
   */
  ofstream config_file_stream(config_file_, std::ios::out | std::ios::trunc);
  if (config_file_stream.is_open() == false) {
    logger.log(LOG_ERR, "Can't initialize WU config file.");
    return false;
  }

  /*
   * Define how we are going to build the json file
   */
  StreamWriterBuilder builder;
  builder["commentStyle"] = "None";
  builder["indentation"] = "   ";  // or "\t" for tabs

  /*
   * Get a stream writer pointer and do the wite.
   * unique_ptr will get free'd when it goes out of scope.
   */
  unique_ptr<StreamWriter> writer(builder.newStreamWriter());
  writer->write(data, &config_file_stream);

  /*
   * Close the stream and unlock
   */
  config_file_stream.close();
  config_guard.unlock();

  return true;
}

string WeatherStationConfig::getLockFileName(string file) {
  std::filesystem::path file_path = file;

  string fname = file_path.filename();

  string lock_file(lock_directory);
  lock_file += fname + string(lock_file_suffix);

  return lock_file;
}
