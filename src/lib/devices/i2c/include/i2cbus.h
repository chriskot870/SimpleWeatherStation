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
#include <mutex>
#include <string>

using std::lock_guard;
using std::mutex;
using std::string;

namespace qw_devices {

enum I2cBusStatus { I2CBUS_STATUS_OK,
                    I2CBUS_STATUS_NODEV,
                    I2CBUS_STATUS_UNKNOWN_FUNCTIONS,
                    I2CBUS_STATUS_UNDEFINED };

/*
 * I2C device name prefix.
 * all I2C device names begin with this value
 */
constexpr string i2c_devicename_prefix = "/dev/i2c-";

class I2cBus {

 public:
  I2cBus(string bus_name);

  /*
   * This is for use with devices that use a command/result model. This writes a
   * command to the slave_address. It then reads from the slave address to get
   * the results of that command. Typically there is a delay between writing the
   * command and reading the result.
   * The sht4x works this way.
   */
  int writeCommand(uint8_t slave_address, uint8_t* command, uint8_t count);

  /*
   * This command covers devices that first send a command via a write. The
   * subsequent read of just the slave address will return the results.
   * These often require a delay to give some time for the command to
   * get executed on the device. It is then followed by a read.
   * This is how the sht4x device operates.
   */
  int readCommandResult(uint8_t slave_address, uint8_t* buffer, uint8_t count);

  /*
   * This routine copies multiple address' from the device to memory
   * The sht4x works this way.
   */
  int transferDataFromRegisters(uint8_t slave_address, uint8_t sub_adress,
                                uint8_t* buffer, uint8_t count);

  /*
   * This routine copies data from memory to multiple sub-address' in the device
   * The sht4x works this way.
   */
  int transferDataToRegisters(uint8_t slave_address, uint8_t sub_adress,
                              uint8_t* buffer, uint8_t count);

  /*
   * This routine returns the device name
   */
  string busName();

  I2cBusStatus status();

 private:
  static mutex i2cbus_lock;

  string bus_device_name_;

  unsigned long i2c_functions_ = 0;

  I2cBusStatus status_ = I2CBUS_STATUS_OK;
};

}  // Namespace qw_devices

#endif  // SRC_LIB_DEVICES_I2C_INCLUDE_I2CBUS_H_