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

#ifndef LIB_UTILITIES_SYSTEM_SD_BUS_H_
#define LIB_UTILITIES_SYSTEM_SD_BUS_H_

#include <fmt/format.h>
#include <systemd/sd-bus.h>
#include <algorithm>
#include <array>
#include <expected>
#include <map>
#include <string>
#include <variant>
#include <vector>

using std::expected;
using std::string;
using std::unexpected;
using std::variant;

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
  string* name = nullptr;
  string* message = nullptr;
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
  SdBusService(string name, SdBus bus);

  string name_;
  SdBus bus_;
};

class SdBusObject {
 public:
  SdBusObject(string name, const SdBusService& sdbus_service);

  string name_;
  SdBusService sdbus_service_;
};

class SdBusInterface {
 public:
  SdBusInterface(string name, const SdBusObject& sdbus_object);

  string name_;
  SdBusObject sdbus_object_;
};

class SdBusMethod {

 public:
  SdBusMethod(string name, string signature, string result_value, string flags,
              const SdBusInterface& sdbus_interface);

  string name_;
  string signature_;
  string flags_;
  SdBusInterface sdbus_interface_;
};

class SdBusProperty {
 public:
  SdBusProperty(string name, string signature, string flags,
                const SdBusInterface& sdbus_interface);

  expected<variant<SdBusNumericResult, string>, SdBusError> getValue();

  string name_;
  string signature_;
  string flags_;
  SdBusInterface sdbus_interface_;

 private:
  expected<string, SdBusError> process_char_type_message(sd_bus_message* m);

  expected<SdBusNumericResult, SdBusError> process_number_type_message(
      sd_bus_message* m);
};

#endif  // LIB_UTILITIES_SYSTEM_SD_BUS_H_
