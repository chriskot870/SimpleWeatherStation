
#include "weather_station.h"

extern Logger logger;

using std::ifstream;
using std::ofstream;
using std::string;
using std::unique_ptr;

WeatherStationConfig::WeatherStationConfig(const string& config_file)
    : config_file_(config_file) {}

void WeatherStationConfig::setConfigFile(string config_file) {

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

bool WeatherStationConfig::initialize() {

  Json::Value initial_data;
  Json::Reader initial_config_reader;

  if (initial_config_reader.parse(ws_default_config, initial_data) == false) {
    logger.log(LOG_ERR,
               "Failed to parse config Weather Undergroubd gonfig file.");
    return false;
  }

  bool val = putRoot(initial_data);

  return val;
}

bool WeatherStationConfig::getRoot(Json::Value& ws_json_config) {

  Json::Reader json_config_reader;
  string lock_file = getLockFileName(config_file_);

  LockingFile config_guard(lock_file);

  config_guard.lock();
  ifstream config_file_stream(config_file_);
  if (config_file_stream.is_open() == false) {
    logger.log(LOG_ERR, "Failed to open the WU config file.");
    exit(1);
  }

  if (json_config_reader.parse(config_file_stream, ws_json_config) == false) {
    logger.log(LOG_ERR,
               "Failed to parse config Weather Undergroubd gonfig file.");
    return false;
  }
  config_file_stream.close();
  config_guard.unlock();

  return true;
}

bool WeatherStationConfig::putRoot(Json::Value data) {

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
  Json::StreamWriterBuilder builder;
  builder["commentStyle"] = "None";
  builder["indentation"] = "   ";  // or "\t" for tabs

  /*
   * Get a stream writer pointer and do the wite.
   * unique_ptr will get free'd when it goes out of scope.
   */
  unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
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

  string lock_file = ws_lock_directory + fname + ws_lock_file_suffix;

  return lock_file;
}
