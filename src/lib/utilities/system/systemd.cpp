
#include "systemd.h"

using fmt::format;
using std::find;
using std::vector;

/*
 * Technically this should be a directory that has a separate file for each
 * of these classes. But, we only want a couple of properties and really just
 * just need the names. If actually need more functionality we can make
 * the systemd directory and break the classes out into it.
 * 
 * Since we really just want the name we make the class properties public
 * and just access them directly.
 */
const std::array<string, 10> sd_bus_number_type_message_list = {
    "y", "b", "n", "q", "i", "u", "x", "t", "d", "h"};
const std::array<string, 3> sd_bus_char_type_message_list = {"s", "o", "g"};

SdBus::SdBus(SdBusType type) : type_(type) {}

SdBusService::SdBusService(string name, SdBus bus) : name_(name), bus_(bus) {}

SdBusObject::SdBusObject(string name, const SdBusService& sdbus_service)
    : name_(name), sdbus_service_(sdbus_service) {}

SdBusInterface::SdBusInterface(string name, const SdBusObject& sdbus_object)
    : name_(name), sdbus_object_(sdbus_object) {}

SdBusMethod::SdBusMethod(string name, string signature, string result_value,
                         string flags, const SdBusInterface& sdbus_interface)
    : name_(name),
      signature_(signature),
      flags_(flags),
      sdbus_interface_(sdbus_interface) {}

SdBusProperty::SdBusProperty(string name, string signature, string flags,
                             const SdBusInterface& sdbus_interface)
    : name_(name),
      signature_(signature),
      flags_(flags),
      sdbus_interface_(sdbus_interface) {}

expected<variant<SdBusNumericResult, string>, SdBusError>
SdBusProperty::getValue() {

  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus_message* m = NULL;
  sd_bus* bus = NULL;
  int r = EINVAL;
  SdBusError return_error;

  if ((find(sd_bus_char_type_message_list.begin(),
            sd_bus_char_type_message_list.end(),
            signature_) != sd_bus_char_type_message_list.end()) &&
      (find(sd_bus_number_type_message_list.begin(),
            sd_bus_number_type_message_list.end(),
            signature_) != sd_bus_number_type_message_list.end())) {
    return_error.type = SD_BUS_PARAM_ERROR;
    return_error.code = EINVAL;
    return_error.name = new string("Invalid signature for property");
    return_error.message =
        new string(format("Unsupported Property signature {}", signature_));
    return_error.need_free = true;
    return unexpected(return_error);
  }

  switch (sdbus_interface_.sdbus_object_.sdbus_service_.bus_.type_) {
    case SD_BUS_TYPE_SYSTEM:
      r = sd_bus_open_system(&bus);
      break;
    case SD_BUS_TYPE_USER:
      r = sd_bus_open_user(&bus);
      break;
    default:
      return_error.type = SD_BUS_BUS_ERROR;
      return_error.code = EINVAL;
      return_error.name = new string("Invalid SD Bus Type");
      return_error.message = new string(
          format("SD Bus Type Invalid {}",
                 sdbus_interface_.sdbus_object_.sdbus_service_.bus_.type_));
      return_error.need_free = true;
      return unexpected(return_error);
  }
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
  r = sd_bus_get_property(
      bus, /* The bus */
      sdbus_interface_.sdbus_object_.sdbus_service_.name_
          .c_str(),                                 /* The service */
      sdbus_interface_.sdbus_object_.name_.c_str(), /*The object */
      sdbus_interface_.name_.c_str(),               /* The interface */
      name_.c_str(),                                /* The property member */
      &error,              /* Where errors are stored */
      &m,                  /* Return message */
      signature_.c_str()); /* The signature */
  if (r < 0) {
    sd_bus_unref(bus);
    return_error.type = SD_BUS_EXEC_ERROR;
    return_error.code = r;
    return_error.name = new string(error.name);
    return_error.message = new string(error.message);
    return_error.need_free = true;
    sd_bus_error_free(&error);
    return unexpected(return_error);
  }
  /*
   * I am done with the bus and error so clean thrm up
   */
  sd_bus_error_free(&error);
  sd_bus_unref(bus);

  if (find(sd_bus_char_type_message_list.begin(),
           sd_bus_char_type_message_list.end(),
           signature_) != sd_bus_char_type_message_list.end()) {
    expected<string, SdBusError> msg = process_char_type_message(m);
    /*
     * I am done with the message buffer so clean it uo
     */
    sd_bus_message_unref(m);
    return msg;
  }
  if (find(sd_bus_number_type_message_list.begin(),
           sd_bus_number_type_message_list.end(),
           signature_) != sd_bus_number_type_message_list.end()) {
    expected<SdBusNumericResult, SdBusError> msg =
        process_number_type_message(m);
    /*
     * I am done with the message buffer so clean it uo
     */
    sd_bus_message_unref(m);
    return msg;
  }

  return_error.type = SD_BUS_MESSAGE_ERROR;
  return_error.code = r;
  return_error.name = new string("SD Bus Message Unsupported Signature");
  return_error.message = new string(
      format("SD Bus Property has Invalid Signature {}", signature_));
  return_error.need_free = true;
  return unexpected(return_error);
}

expected<string, SdBusError> SdBusProperty::process_char_type_message(
    sd_bus_message* m) {

  SdBusError return_error;
  int r;
  /*
   * All the char types want a const char ** so we just need the one type
   */
  char* output_chr;

  r = sd_bus_message_read(m, signature_.c_str(), &output_chr);
  if (r < 0) {
    return_error.type = SD_BUS_MESSAGE_ERROR;
    return_error.code = r;
    return_error.name = new string("SD Bus Message Interpretation Failure");
    return_error.message =
        new string(format("SD Bus sbus_message_read() Failed for signature "
                          "{:1}. Error Code: {:2}",
                          signature_, r));
    return_error.need_free = true;
    return unexpected(return_error);
  }
  string output = output_chr;
  return output;
}

expected<SdBusNumericResult, SdBusError>
SdBusProperty::process_number_type_message(sd_bus_message* m) {

  SdBusError return_error;
  int r;

  /*
   * Different signatures require different types;
   */

  SdBusNumericResult val;

  r = sd_bus_message_read(m, signature_.c_str(), &val);
  if (r < 0) {
    return_error.type = SD_BUS_MESSAGE_ERROR;
    return_error.code = r;
    return_error.name = new string("SD Bus Message Interpretation Failure");
    return_error.message = new string(format(
        "SD Bus sbus_message_read() Failed for signature {}. Error Code: {}",
        signature_, r));
    return_error.need_free = true;
    return unexpected(return_error);
  }
  return val;
}
