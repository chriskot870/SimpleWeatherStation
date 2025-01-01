/*
 * get the Weather Underground access information
 * update the weather underground configuration file
 * with the data.
 */
#include <chrono>
#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <systemd/sd-journal.h>
#include <jsoncpp/json/json.h>
#include <filesystem>
#include <fcntl.h>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::ifstream;

enum MODE {
    MAP,
    OS,
    FILEHANDLE,
    STREAM,
    MODE_MAX
};

void mmap_lock() {
  struct stat fd_stat;
  Json::Reader wu_config;
  Json::Value wu_access;

  auto start = high_resolution_clock::now();
  int fd = open(weather_station_config.c_str(), O_RDONLY);
  if ( fd == -1) {
    printf("Couldn't open file %s\n", weather_station_config.c_str());
    exit(1);
  }

  if (flock(fd, LOCK_EX) != 0) {
    printf("Couldn't lock the configuration file\n");
    exit (1);
  }

  if (fstat(fd, &fd_stat) != 0) {
    printf("Couldn't stat the configuration file\n");
    exit (1);
  };

  char *buffer = (char *)mmap(NULL, fd_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (buffer == MAP_FAILED) {
    printf("Couldn't mmap the configuration file\n");
    exit (1);
  }

  if (wu_config.parse(buffer, buffer + fd_stat.st_size, wu_access) == false) {
    printf("Failed to parse config Weather Underground config file.");
    exit(1);
  }

  if (munmap((void *)buffer, fd_stat.st_size) != 0) {
    printf("Failed to unmap config file.");
    exit(1);
  }
  //closing the file will unlock it.
  close(fd);

  auto end = high_resolution_clock::now();

  auto elapsed = duration_cast<microseconds>(end - start);

  printf("Elapsed Time: %ld microseconds\n", elapsed.count());

  printf("Name: %s\n", wu_access["pwu_name"].asString().c_str());
  printf("Password: %s\n", wu_access["pwu_password"].asString().c_str());

  return;
}

void mmap_nolock() {
  struct stat fd_stat;
  Json::Reader wu_config;
  Json::Value wu_access;

  auto start = high_resolution_clock::now();
  int fd = open(weather_station_config.c_str(), O_RDONLY);
  if ( fd == -1) {
    printf("Couldn't open file %s\n", weather_station_config.c_str());
    exit(1);
  }

  if (fstat(fd, &fd_stat) != 0) {
    printf("Couldn't stat the configuration file\n");
    exit (1);
  };

  char *buffer = (char *)mmap(NULL, fd_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (buffer == MAP_FAILED) {
    printf("Couldn't mmap the configuration file\n");
    exit (1);
  }

  if (wu_config.parse(buffer, buffer + fd_stat.st_size, wu_access) == false) {
    printf("Failed to parse config Weather Underground config file.");
    exit(1);
  }

  if (munmap((void *)buffer, fd_stat.st_size) != 0) {
    printf("Failed to unmap config file.");
    exit(1);
  }
  //closing the file will unlock it.
  close(fd);

  auto end = high_resolution_clock::now();

  auto elapsed = duration_cast<microseconds>(end - start);

  printf("Elapsed Time: %ld microseconds\n", elapsed.count());

  printf("Name: %s\n", wu_access["pwu_name"].asString().c_str());
  printf("Password: %s\n", wu_access["pwu_password"].asString().c_str());

  return;
}

void oscalls_lock() {
  struct stat fd_stat;
  Json::Reader wu_config;
  Json::Value wu_access;

  auto start = high_resolution_clock::now();
  int fd = open(weather_station_config.c_str(), O_RDONLY);
  if ( fd == -1) {
    printf("Couldn't open file %s\n", weather_station_config.c_str());
    exit(1);
  }

  if (flock(fd, LOCK_EX) != 0) {
    printf("Couldn't lock the configuration file\n");
    exit (1);
  }

  if (fstat(fd, &fd_stat) != 0) {
    printf("Couldn't stat the configuration file\n");
    exit (1);
  };

  char* buffer = new char[fd_stat.st_size];

  if (read(fd, buffer, fd_stat.st_size) != fd_stat.st_size) {
    printf("Read failure of config file\n");
    exit(1);
  }

  if (wu_config.parse(buffer, buffer + fd_stat.st_size, wu_access) == false) {
    printf("Failed to parse config Weather Underground config file.");
    exit(1);
  }

  delete buffer;

  //closing the file will unlock it.
  close(fd);

  auto end = high_resolution_clock::now();

  auto elapsed = duration_cast<microseconds>(end - start);

  printf("Elapsed Time: %ld microseconds\n", elapsed.count());

  printf("Name: %s\n", wu_access["pwu_name"].asString().c_str());
  printf("Password: %s\n", wu_access["pwu_password"].asString().c_str());

  return;
}

void oscalls_nolock() {
  struct stat fd_stat;
  Json::Reader wu_config;
  Json::Value wu_access;

  auto start = high_resolution_clock::now();
  int fd = open(weather_station_config.c_str(), O_RDONLY);
  if ( fd == -1) {
    printf("Couldn't open file %s\n", weather_station_config.c_str());
    exit(1);
  }

  if (fstat(fd, &fd_stat) != 0) {
    printf("Couldn't stat the configuration file\n");
    exit (1);
  };

  char* buffer = new char[fd_stat.st_size];

  if (read(fd, buffer, fd_stat.st_size) != fd_stat.st_size) {
    printf("Read failure of config file\n");
    exit(1);
  }

  if (wu_config.parse(buffer, buffer + fd_stat.st_size, wu_access) == false) {
    printf("Failed to parse config Weather Underground config file.");
    exit(1);
  }

  delete buffer;

  //closing the file will unlock it.
  close(fd);

  auto end = high_resolution_clock::now();

  auto elapsed = duration_cast<microseconds>(end - start);

  printf("Elapsed Time: %ld microseconds\n", elapsed.count());

  printf("Name: %s\n", wu_access["pwu_name"].asString().c_str());
  printf("Password: %s\n", wu_access["pwu_password"].asString().c_str());

  return;
}

void filehandle_lock() {
  struct stat fd_stat;
  Json::Reader wu_config;
  Json::Value wu_access;

  auto start = high_resolution_clock::now();
  FILE* fh = fopen(weather_station_config.c_str(), "r");
  if ( fh == NULL) {
    printf("Couldn't open file %s\n", weather_station_config.c_str());
    exit(1);
  }

  if (flock(fileno(fh), LOCK_EX) != 0) {
    printf("Couldn't lock the configuration file\n");
    exit (1);
  }

  if (fstat(fileno(fh), &fd_stat) != 0) {
    printf("Couldn't stat the configuration file\n");
    exit (1);
  };

  char* buffer = new char[fd_stat.st_size];

  if (fread(buffer, sizeof(char), fd_stat.st_size, fh) != fd_stat.st_size) {
    printf("Read failure of config file\n");
    exit(1);
  }

  if (wu_config.parse(buffer, buffer + fd_stat.st_size, wu_access) == false) {
    printf("Failed to parse config Weather Underground config file.");
    exit(1);
  }

  delete buffer;

  //closing the file will unlock it.
  fclose(fh);

  auto end = high_resolution_clock::now();

  auto elapsed = duration_cast<microseconds>(end - start);

  printf("Elapsed Time: %ld microseconds\n", elapsed.count());

  printf("Name: %s\n", wu_access["pwu_name"].asString().c_str());
  printf("Password: %s\n", wu_access["pwu_password"].asString().c_str());

  return;
}

void filehandle_nolock() {
  struct stat fd_stat;
  Json::Reader wu_config;
  Json::Value wu_access;

  auto start = high_resolution_clock::now();
  FILE* fh = fopen(weather_station_config.c_str(), "r");
  if ( fh == NULL) {
    printf("Couldn't open file %s\n", weather_station_config.c_str());
    exit(1);
  }

  if (fstat(fileno(fh), &fd_stat) != 0) {
    printf("Couldn't stat the configuration file\n");
    exit (1);
  };

  char* buffer = new char[fd_stat.st_size];

  if (fread(buffer, sizeof(char), fd_stat.st_size, fh) != fd_stat.st_size) {
    printf("Read failure of config file\n");
    exit(1);
  }

  if (wu_config.parse(buffer, buffer + fd_stat.st_size, wu_access) == false) {
    printf("Failed to parse config Weather Underground config file.");
    exit(1);
  }

  delete buffer;

  fclose(fh);

  auto end = high_resolution_clock::now();

  auto elapsed = duration_cast<microseconds>(end - start);

  printf("Elapsed Time: %ld microseconds\n", elapsed.count());

  printf("Name: %s\n", wu_access["pwu_name"].asString().c_str());
  printf("Password: %s\n", wu_access["pwu_password"].asString().c_str());

  return;
}

void stream_nolock() {
  Json::Reader wu_config;
  Json::Value wu_access;

  auto start = high_resolution_clock::now();

  ifstream wu_config_file(weather_station_config);
  if (wu_config_file.is_open() == false) {
    printf("Failed to open the WU config file.");
    exit(1);
  }
  
  if (wu_config.parse(wu_config_file, wu_access) == false) {
    printf("Failed to parse config Weather Underground config file.");
    exit(1);
  }
  // This close will release the lock
  wu_config_file.close();
  auto end = high_resolution_clock::now();

  auto elapsed = duration_cast<microseconds>(end - start);

  printf("Elapsed Time: %ld microseconds\n", elapsed.count());

  printf("Name: %s\n", wu_access["pwu_name"].asString().c_str());
  printf("Password: %s\n", wu_access["pwu_password"].asString().c_str());

  return;
}

int main(int argc, char** argv) {
  int opt;
  MODE mode = MODE_MAX;
  bool lock = false;

  if ( argc )

  while((opt = getopt(argc, argv, "mohsl")) != -1) {
    switch (opt) {
      case 'm' :
        mode = MAP;
        break;
      case 'o' :
        mode = OS;
        break;
      case 'h' :
        mode = FILEHANDLE;
        break;
      case 's' :
        mode = STREAM;
        break;
      case 'l' :
        lock = true;
        break;
    }
  }
  if (mode == MODE_MAX) {
    printf("Usage -[mohsl] <-l>");
    exit(1);
  }

  if (mode == OS) {
    if (lock == true) {
      printf("Running oscalls_lock()\n");
      oscalls_lock();
    }else {
      printf("Running oscalls_nolock()\n");
      oscalls_nolock();
    }
  }
  if (mode == MAP) {
    if (lock == true) {
      printf("Running mmap_lock()\n");
      mmap_lock();
    }else {
      printf("Running mmap_nolock()\n");
      mmap_nolock();
    }
  }
  if (mode == FILEHANDLE) {
    if (lock == true) {
      printf("Running filehandle_lock()\n");
      filehandle_lock();
    }else {
      printf("Running filehandle_nolock()\n");
      filehandle_nolock();
    }
  }
  if (mode == STREAM) {
    if (lock == true) {
      printf("Stream has no locking option\n");
    } else {
      printf("Running stream_nolock()\n");
      stream_nolock();
    }
  }

  exit(0);
}