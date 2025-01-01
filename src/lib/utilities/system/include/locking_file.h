/*
 * Copyright 2024 Chris Kottaridis
 */

#ifndef LIB_UTILITIES_SYSTEM_LOCKINGFILE_H_
#define LIB_UTILITIES_SYSTEM_LOCKINGFILE_H_

#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

using std::string;

class LockingFile {

 public:
  LockingFile(string lockfile);

  bool lock();

  void unlock();

  ~LockingFile();

 private:
  int fd_ = -1;
  string lockfile_;

};

#endif  // LIB_UTILITIES_SYSTEM_LOCKINGFILE_H_