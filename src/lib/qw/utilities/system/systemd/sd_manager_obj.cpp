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

#include "sd_manager_obj.h"

using fmt::format;

SdManager::SdManager(string destination, string path, string interface) :
  destination_(destination), path_(path), interface_(interface) {
}

/*
 * StartUnit(in  s name,
 *           in  s mode,
 *           out o job);
 */
expected<string, SdBusError> SdManager::StartUnit(string name, string mode) {
  sd_bus_error error = SD_BUS_ERROR_NULL;
   sd_bus_message *m = NULL;
   sd_bus* bus = NULL;
   SdBusError return_error;
   int r;
   string in_signature = "ss"; // The signature for StopUnit method is "ss"
   string out_signature;
   char* job;

   /*
    * Open the system bus
    */
   r = sd_bus_open_system(&bus);
   if (r < 0) {
    sd_bus_unref(bus);
    return_error.type = SD_BUS_BUS_ERROR;
    return_error.code = r;
    return_error.name = new string("SD Bus initialization Failure");
    return_error.message =
        new string(format("SD Bus failed to initialize. Error Code: {}", r));
    return_error.need_free = true;
    return unexpected(return_error);
   }

   /*
    * Get the property
    */
   r = sd_bus_call_method(
      bus,
      destination_.c_str(),  /* The destination */
      path_.c_str(),         /* The path*/
      interface_.c_str(),    /* The interface */
      "StartUnit",           /* The method StatUnit */
      &error,                /* Where errors are stored */
      &m,                    /* Return message */
      in_signature.c_str(),
      name.c_str(),
      mode.c_str()
    );            /* The signature  */
   if (r < 0) {
    sd_bus_unref(bus);
    return_error.type = SD_BUS_EXEC_ERROR;
    return_error.code = r;
    return_error.name = new string(error.name);
    return_error.message = new string(error.message);
    return_error.need_free = true;
    sd_bus_message_unref(m);
    sd_bus_error_free(&error);
    return unexpected(return_error);
  }

  /*
   * I am done with the bus and error so clean them up
   */
  sd_bus_error_free(&error);
  sd_bus_unref(bus);

  /*
   * Convert the message to a string
   */
  r = sd_bus_message_read(m, out_signature.c_str(), &job);
  sd_bus_message_unref(m);
  if (r < 0) {
    return_error.type = SD_BUS_MESSAGE_ERROR;
    return_error.code = r;
    return_error.name = new string("SD Bus Message Interpretation Failure");
    return_error.message =
        new string(format("SD Bus sbus_message_read() Failed for signature "
                          "{:1}. Error Code: {:2}",
                          out_signature, r));
    return_error.need_free = true;
    return unexpected(return_error);
  }
  string output(job);

  //free(job);
  /*
   * return the string result
   */
  return output;
}

/*
 * StopUnit(in  s name,
 *          in  s mode,
 *          out o job);
 */
expected<string, SdBusError> SdManager::StopUnit(string name, string mode) {
  sd_bus_error error = SD_BUS_ERROR_NULL;
   sd_bus_message *m = NULL;
   sd_bus* bus = NULL;
   SdBusError return_error;
   int r;
   string in_signature = "ss"; // The signature for StopUnit method is "ss"
   string out_signature;
   char* job;

   /*
    * Open the system bus
    */
   r = sd_bus_open_system(&bus);
   if (r < 0) {
    sd_bus_unref(bus);
    return_error.type = SD_BUS_BUS_ERROR;
    return_error.code = r;
    return_error.name = new string("SD Bus initialization Failure");
    return_error.message =
        new string(format("SD Bus failed to initialize. Error Code: {}", r));
    return_error.need_free = true;
    return unexpected(return_error);
   }

   /*
    * Get the property
    */
   r = sd_bus_call_method(
      bus,
      destination_.c_str(),  /* The destination */
      path_.c_str(),         /* The path*/
      interface_.c_str(),    /* The interface */
      "StopUnit",            /* The method StopUnit */
      &error,                /* Where errors are stored */
      &m,                    /* Return message */
      in_signature.c_str(),
      name.c_str(),
      mode.c_str()
    );            /* The signature  */
   if (r < 0) {
    return_error.type = SD_BUS_EXEC_ERROR;
    return_error.code = r;
    return_error.name = new string(error.name);
    return_error.message = new string(error.message);
    return_error.need_free = true;
    sd_bus_unref(bus);
    sd_bus_message_unref(m);
    sd_bus_error_free(&error);
    return unexpected(return_error);
  }

  /*
   * I am done with the bus and error so clean them up
   */
  sd_bus_error_free(&error);
  sd_bus_unref(bus);

  /*
   * Convert the message to a string
   */
  r = sd_bus_message_read(m, out_signature.c_str(), &job);
  sd_bus_message_unref(m);
  if (r < 0) {
    return_error.type = SD_BUS_MESSAGE_ERROR;
    return_error.code = r;
    return_error.name = new string("SD Bus Message Interpretation Failure");
    return_error.message =
        new string(format("SD Bus sbus_message_read() Failed for signature "
                          "{:1}. Error Code: {:2}",
                          out_signature, r));
    return_error.need_free = true;
    return unexpected(return_error);
  }
  string output(job);

  //free(job);

  /*
   * return the string result
   */
  return output;
}
