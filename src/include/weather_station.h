/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains data for the weather station
 */

#ifndef SRC_INCLUDE_WEATHER_STATION_H_
#define SRC_INCLUDE_WEATHER_STATION_H_

#include <fcntl.h>
#include <jsoncpp/json/json.h>
#include <poll.h>
#include <sys/file.h>
#include <sys/inotify.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <systemd/sd-journal.h>
#include <unistd.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <variant>

#include "locking_file.h"
#include "logger.h"
#include "systemd.h"

#include "weather_station_config.h"

using std::map;
using std::string;

const string args_log_mode_journal = "journal";
const string args_log_mode_file = "file";
const string args_log_mode_nologging = "none";

#endif  // SRC_INCLUDE_WEATHER_STATION_H
