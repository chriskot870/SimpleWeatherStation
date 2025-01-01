
#include "locking_file.h"

LockingFile::LockingFile(string lockfile) : lockfile_(lockfile) {}

bool LockingFile::lock() {

  fd_= open(lockfile_.c_str(), O_RDONLY | O_CREAT);
  if (fd_ == -1) {
    return false;
  }

  int val = flock(fd_, LOCK_EX);
  if (val != 0) {
    return false;
  }

  return true;
}

void LockingFile::unlock() {

  if (fd_ != -1) {
    close(fd_);
    fd_= -1;
  }
   return;
}

LockingFile::~LockingFile() {
  
  unlock();

}

