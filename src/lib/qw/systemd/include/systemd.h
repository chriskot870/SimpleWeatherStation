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

#ifndef SRC_LIB_QW_SYSTEMD_INCLUDE_SYSTEMD_H_
#define SRC_LIB_QW_SYSTEMD_INCLUDE_SYSTEMD_H_

#include <fmt/format.h>
#include <systemd/sd-bus.h>
#include <algorithm>
#include <array>
#include <expected>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace qw::systemd {

const std::string systemd_destination = "org.freedesktop.systemd1";
const std::string systemd_quietwind_service_path =
    "/org/freedesktop/systemd1/unit/quietwind_2eweather_2eservice";
const std::string systemd_unit_interface = "org.freedesktop.systemd1.Unit";
const std::string systemd_service_interface =
    "org.freedesktop.systemd1.Service";

enum SdBusErrorType {
  SD_BUS_NO_ERROR,
  SD_BUS_PARAM_ERROR,
  SD_BUS_BUS_ERROR,
  SD_BUS_EXEC_ERROR,
  SD_BUS_MESSAGE_ERROR,
};

enum SdBusType { SD_BUS_TYPE_SYSTEM, SD_BUS_TYPE_USER };

struct SdBusError {
  /*
   * Mae sure these have some values when created
   */
  SdBusErrorType type = SD_BUS_NO_ERROR;
  int code = 0;
  std::string* name = nullptr;
  std::string* message = nullptr;
  bool need_free = false;
  /*
   * If you need to reset things use clear() 
   */
  void clear() {
    free();
    type = SD_BUS_NO_ERROR;
    code = 0;
    name = nullptr;
    message = nullptr;
    need_free = false;
  }
  /*
   * If you need to free the name and message pointers.
   * Note it checks the value of need_free. It needs to be
   * correct so we don't try nd free a nullptr.
   */
  void free() {
    if (need_free) {
      delete name;
      delete message;
      need_free = false;
    }
  }
  /*
   * If this goes out of scope be sure to free name and message
   * if need be. 
   */
  ~SdBusError() { free(); }
};

const std::array<std::string, 19> systemd_valid_signatures = {
    "y",  // 8-bit unsigned integer
    "b",  // boolean value
    "n",  // 16-bit signed integer
    "q",  // 16-bit unsigned integer
    "i",  // 32-bit signed integer
    "u",  // 32-bit unsigned integer
    "x",  // 64-bit signed integer
    "t",  // 64-bit unsigned integer
    "d",  // double-precision floating point (IEEE 754)
    "s",  // UTF-8 string (no embedded nul characters)
    "o",  // D-Bus Object Path string
    "g",  //  D-Bus Signature string
    "a",  // array
    "(",  // structure start
    ")",  // structure end
    "v",  // variant
    "{",  // Dictionary/Map begin
    "}",  // Dictionary/Map end
    "h"   // Unix file descriptor
};

union SdBusNumericResult {
  uint8_t y;
  bool b;
  int16_t n;
  uint16_t q;
  int32_t i;
  uint32_t u;
  int64_t x;
  uint64_t t;
  double d;
};

class SdBus {
 public:
  SdBus(SdBusType type);

  SdBusType type_;
};

class SdBusService {
 public:
  SdBusService(std::string name, SdBus bus);

  std::string name_;
  SdBus bus_;
};

class SdBusObject {
 public:
  SdBusObject(std::string name, const SdBusService& sdbus_service);

  std::string name_;
  SdBusService sdbus_service_;
};

class SdBusInterface {
 public:
  SdBusInterface(std::string name, const SdBusObject& sdbus_object);

  std::string name_;
  SdBusObject sdbus_object_;
};

class SdBusMethod {

 public:
  SdBusMethod(std::string name, std::string signature, std::string result_value,
              std::string flags, const SdBusInterface& sdbus_interface);

  std::string name_;
  std::string signature_;
  std::string flags_;
  SdBusInterface sdbus_interface_;
};

class SdBusProperty {
 public:
  SdBusProperty(std::string name, std::string signature, std::string flags,
                const SdBusInterface& sdbus_interface);

  std::expected<std::variant<SdBusNumericResult, std::string>, SdBusError>
  getValue();

  std::string name_;
  std::string signature_;
  std::string flags_;
  SdBusInterface sdbus_interface_;

 private:
  std::expected<std::string, SdBusError> process_char_type_message(
      sd_bus_message* m);

  std::expected<SdBusNumericResult, SdBusError> process_number_type_message(
      sd_bus_message* m);
};

}  // namespace qw::systemd

#endif  // SRC_LIB_QW_SYSTEMD_INCLUDE_SYSTEMD_H_
