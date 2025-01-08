
#include "systemd.h"

/*
 * Technically this should be a directory that has a separate file for each
 * of these classes. But, we only want a couple of properties and really just
 * just need the names. If actually need more functionality we can make
 * the systemd directory and break the classes out into it.
 * 
 * Since we really just want the name we make the class properties public
 * and just access them directly.
 */

SdBusService::SdBusService(string name) : name_(name) {}

SdBusObject::SdBusObject(string name, const SdBusService& sdbus_service) :
                         name_(name), sdbus_service_(sdbus_service) {}

SdBusInterface::SdBusInterface(string name, const SdBusObject& sdbus_object) :
                               name_(name), sdbus_object_(sdbus_object) {}

SdBusMethod::SdBusMethod(string name, string signature, string result_value, string flags, const SdBusInterface& sdbus_interface) :
                         name_(name), signature_(signature), flags_(flags), sdbus_interface_(sdbus_interface) {}

SdBusProperty::SdBusProperty(string name, string signature, string flags, const SdBusInterface& sdbus_interface) :
                             name_(name), signature_(signature),
                             flags_(flags), sdbus_interface_(sdbus_interface) {}

int SdBusProperty::getProperty(sd_bus_message **message, sd_bus_error *error) {

    //sd_bus_error error = SD_BUS_ERROR_NULL;
    //sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    int r;

    r = sd_bus_open_system(&bus);
    if (r >= 0) {
     r = sd_bus_get_property(bus,  /* The bus */
                        sdbus_interface_.sdbus_object_.sdbus_service_.name_.c_str(),  /* The service */
                        sdbus_interface_.sdbus_object_.name_.c_str(),  /*The object */
                        sdbus_interface_.name_.c_str(),  /* The interface */
                        name_.c_str(),  /* The property member */
                        error,  /* Where errors are stored */
                        message,  /* Return message */
                        signature_.c_str()); /* The signature */
    }
    sd_bus_unref(bus);

    return r;
}
