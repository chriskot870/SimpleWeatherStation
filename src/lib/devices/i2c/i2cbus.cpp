/*
 * Copyright 2024 Chris Kottaridis
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>

#include "include/i2cbus.h"

namespace qw_devices {

mutex I2cBus::i2cbus_lock;

I2cBus::I2cBus(string bus_device_name) : bus_device_name_(bus_device_name) {
  int i2c_bus, retval;
  /*
   * We try to access the bus and get the I2C_FUNCS available
   */
  i2c_bus = open(bus_device_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    /*
     * If the OS open() failed set status to NODEV
     */
    status_ = NODEV;
  }

  retval = ioctl(i2c_bus, I2C_FUNCS, &i2c_functions_);
  close(i2c_bus);
  if (retval != 0) {
    /*
     * We weren't able to get the functions.
     */
    status_ = UNKNOWN_FUNCTIONS;
  }

  return;
};

string I2cBus::busName() {

  return bus_device_name_;
}

int I2cBus::transferDataToRegisters(uint8_t slave_address, uint8_t reg,
                                    uint8_t* buffer, uint8_t count) {
  int retval;
  int i2c_bus;
  struct i2c_msg fetch_serial_com;
  struct i2c_rdwr_ioctl_data xfer;
  uint8_t xfr_data[255 + 1];

  /*
   * This writes data to the device all within one stop bit.
   */

  lock_guard<mutex> guard(i2cbus_lock);  // Get the lock before accessing the i2cbus

  i2c_bus = open(bus_device_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    /*
     * If the OS open() call fails return the errno it generated
     */
    return errno;
  }

  /*
   * Build the transfer buffer with the register as the first byte
   */
  xfr_data[0] = reg;
  memcpy(&xfr_data[1], buffer, count);

  /*
   * Write to the registers
   */
  fetch_serial_com.addr = slave_address;
  fetch_serial_com.flags = 0; /* Do a write */
  // Write the register and then the count of bytes in the buffer
  fetch_serial_com.len = count + 1;
  fetch_serial_com.buf = xfr_data;

  xfer.msgs = &fetch_serial_com;
  xfer.nmsgs = 1;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  close(i2c_bus);

  if (retval != 1) {
    if (retval == -1) {
      /*
       * If the ioctl() call fails return the corresponding errno
       */
      return errno;
    }
    /*
     * If it doesn't return -1 or 1 then resturn EIO
     */
    return EIO;
  }

  return 0;
}

int I2cBus::transferDataFromRegisters(uint8_t slave_address, uint8_t reg,
                                      uint8_t* buffer, uint8_t count) {
  /*
   * This writes to the slave address and reads the register and all subsequent registers up to count.
   */
  int retval, i2c_bus;
  struct i2c_msg fetch_serial_com[2];
  struct i2c_rdwr_ioctl_data xfer;

  lock_guard<mutex> guard(i2cbus_lock);  // Get the lock before accessing the i2cbus

  i2c_bus = open(bus_device_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    /*
     * If the OS open() call fails return the errno it generated
     */
    return errno;
  }

  /*
   * Write the first regiater
   */
  fetch_serial_com[0].addr = slave_address;
  fetch_serial_com[0].flags = 0; /* Do a write to define the first register */
  fetch_serial_com[0].len = 1;
  fetch_serial_com[0].buf = &reg;

  /*
   * Now read that register and count following it.
   */
  fetch_serial_com[1].addr = slave_address;
  fetch_serial_com[1].flags = I2C_M_RD; /* Do a read */
  fetch_serial_com[1].len = count;
  fetch_serial_com[1].buf = buffer;

  xfer.msgs = fetch_serial_com;
  xfer.nmsgs = 2;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  close(i2c_bus);
  if (retval != 2) {
    return errno;
  }

  return 0;
}

int I2cBus::writeCommand(uint8_t slave_address, uint8_t* command,
                         uint8_t count) {
  int i2c_bus, retval;
  struct i2c_msg fetch_serial_com;
  struct i2c_rdwr_ioctl_data xfer;

  lock_guard<mutex> guard(i2cbus_lock);  // Get the lock before accessing the i2cbus

  i2c_bus = open(bus_device_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    return errno;
  }

  /*
   * Write the command to get a measurement
   */
  fetch_serial_com.addr = slave_address;
  fetch_serial_com.flags = 0; /* Do a write */
  fetch_serial_com.len = count;
  fetch_serial_com.buf = command;

  xfer.msgs = &fetch_serial_com;
  xfer.nmsgs = 1;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  close(i2c_bus);
  if (retval != 1) {
    return errno;
  }

  return 0;
}

int I2cBus::readCommandResult(uint8_t slave_address, uint8_t* buffer,
                              uint8_t count) {
  int i2c_bus, retval;
  struct i2c_msg fetch_serial_com;
  struct i2c_rdwr_ioctl_data xfer;

  lock_guard<mutex> guard(i2cbus_lock);  // Get the lock before accessing the i2cbus

  i2c_bus = open(bus_device_name_.c_str(), O_RDWR);
  if (i2c_bus < 0) {
    return errno;
  }

  /*
   * Perform the read to get the measurement
   */
  fetch_serial_com.addr = slave_address;
  fetch_serial_com.flags = I2C_M_RD;
  fetch_serial_com.len = count;
  fetch_serial_com.buf = buffer;

  xfer.msgs = &fetch_serial_com;
  xfer.nmsgs = 1;

  retval = ioctl(i2c_bus, I2C_RDWR, &xfer);
  close(i2c_bus);
  if (retval != 1) {
    return errno;
  }

  return 0;
}

}  // Namespace qw_devices
