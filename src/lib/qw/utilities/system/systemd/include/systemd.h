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

#ifndef LIB_UTILITIES_SYSTEM_SYSTEMD_INCLUDE_SYSTEMD_H_
#define LIB_UTILITIES_SYSTEM_SYSTEMD_INCLUDE_SYSTEMD_H_

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

const string systemd_destination = "org.freedesktop.systemd1";
const string systemd_quietwind_service_path = "/org/freedesktop/systemd1/unit/quietwind_2eweather_2eservice";
const string systemd_unit_interface = "org.freedesktop.systemd1.Unit";
const string systemd_service_interface = "org.freedesktop.systemd1.Service";

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

const std::array<string, 19> systemd_valid_signatures = {
    "y",  // 8-bit unsigned integer
    "b",  // boolean value
    "n",  // 16-bit signed integer
    "q",  // 16-bit unsigned integer
    "i",  // 32-bit signed integer
    "u",  // 32-bit unsigned integer
    "x",  // 64-bit signed integer
    "t",  // 64-bit unsigned integer
    "d", // double-precision floating point (IEEE 754)
    "s", // UTF-8 string (no embedded nul characters)
    "o", // D-Bus Object Path string
    "g",  //  D-Bus Signature string
    "a",  // array
    "(",  // structure start
    ")",  // structure end
    "v",  // variant
    "{",  // Dictionary/Map begin
    "}",  // Dictionary/Map end
    "h"   // Unix file descriptor
};


#endif  // LIB_UTILITIES_SYSTEM_SYSTEMD_INCLUDE_SYSTEMD_H_
