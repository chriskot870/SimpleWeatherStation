/*
 * Copyright 2024 Chris Kottaridis
 */

/*
 * This contains the transfer commands to devices on the i2c bus
 */

#ifndef SRC_LIB_DEVICES_I2C_INCLUDE_I2CBUS_H_
#define SRC_LIB_DEVICES_I2C_INCLUDE_I2CBUS_H_

#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <chrono>
#include <expected>
#include <string>

using std::string;

enum I2cBusStatus { OK, NODEV, UNKNOWN_FUNCTIONS, UNDEFINED };

class I2cBus {

 public:
  I2cBus(string bus_name);

  /**
   * This is for use with devices that use a command/result model. This writes a
   * command to the slave_address. It then reads from the slave address to get
   * the results of that command. Typically there is a delay between wiring the
   * command and reading the result.
   * The sht4x works this way.
   */
  int writeCommand(uint8_t slave_address, uint8_t* command, uint8_t count);

  /**
   * This command covers devices that first send a comman via a write. The
   * subsequent read of just the slave address will return the results.
   * These often require a delay to give some time for the command to
   * get executed on the device. It is then followed by a read.
   * This is how the sht4x device operates.
   */
  int readCommandResult(uint8_t slave_address, uint8_t* buffer, uint8_t count);

  /**
   * This routine copies multiple address' from the device to memory
   */
  int transferDataFromRegisters(uint8_t slave_address, uint8_t sub_adress,
                                uint8_t* buffer, uint8_t count);

  /**
   * This routine copies data from memory to multiple sub-address' in the device
   */
  int transferDataToRegisters(uint8_t slave_address, uint8_t sub_adress,
                              uint8_t* buffer, uint8_t count);

 private:
  string bus_device_name_;

  unsigned long i2c_functions_ = 0;

  I2cBusStatus status_;
};
#endif  // SRC_LIB_DEVICES_I2C_INCLUDE_I2CBUS_H_