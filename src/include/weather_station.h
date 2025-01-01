/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains data for the weather station
 */

#ifndef SRC_INCLUDE_WEATHER_STATION_H_
#define SRC_INCLUDE_WEATHER_STATION_H_

#include <unistd.h>
#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <systemd/sd-journal.h>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <filesystem>
#include <fcntl.h>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>

using std::string;
using std::map;

const string weather_station_config = "/usr/local/etc/qw/wu_access.json";
const string weather_devices_bus = "/dev/i2c-1";
constexpr int ws_loop_wait_ms = ((5 * 60) * 1000);  // 5 minutes in milliseconds

const map<string, string> lock_file_map = { {weather_station_config, "/var/run/wu_access.json"} };

#endif // SRC_INCLUDE_WEATHER_STATION_H
