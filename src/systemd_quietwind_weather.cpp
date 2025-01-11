#include <expected>
#include <variant>

#include "logger.h"
#include "systemd.h"
#include "weather_station.h"
#include "weather_station_config.h"
#include "systemd_quietwind_weather.h"

using std::expected;
using std::unexpected;
using std::variant;
using fmt::format;

extern Logger logger;
/*
 * Set the System Bus, Service and Object we are interested in.
 * We only want to talk to systemd about the quietwind.weather.service
 * in these routines.
 */
const SdBus system_bus(SD_BUS_TYPE_SYSTEM);  // We want the system bus
const SdBusService systemd_service(  // we want to talk to systemd service
      "org.freedesktop.systemd1",
      system_bus);  
// We want to talk to quietwind.weather.service Object within systemd
const SdBusObject sdbus_qw_weather_object(
      "/org/freedesktop/systemd1/unit/quietwind_2eweather_2eservice",
      systemd_service);


expected<string, bool> quietwindWeatherServiceState() {

  string sub_state;
  pid_t main_pid;
  uint32_t burst, directory_mode;
  const char* sub_state_chr;
  const char* type_chr;
  string type;
  int r;
  expected<variant<SdBusNumericResult, string>, SdBusError> value;

  /*
   * Set the Unit interface and properties in that interface that we need
   */
  SdBusInterface sdbus_systemd_unit("org.freedesktop.systemd1.Unit",
                                    sdbus_qw_weather_object);
  SdBusProperty qw_ws_substate("SubState", "s", "emits-change",
                               sdbus_systemd_unit);

  value = qw_ws_substate.getValue();
  if (value.has_value() == false) {
    logger.log(LOG_ERR,
              format("Getting quietwind.weather.service SubState Failed"));
    return unexpected(false);
  }
  /*
   * Make sure it has a string
   */
  if (std::holds_alternative<string>(value.value()) == false) {
    logger.log(LOG_ERR,
              format("Request for SubState did not return a string"));
    return unexpected(false);
  }

  /*
   * It is OK to assign a value
   */
  sub_state = std::get<string>(value.value());

  return sub_state;
}

expected<pid_t, bool> quietwindWeatherServiceMainPID() {

  expected<variant<SdBusNumericResult, string>, SdBusError> value;

  /*
   * Set the Service interface and the properties in that interface that we need
   */
  SdBusInterface sdbus_systemd_service("org.freedesktop.systemd1.Service",
                                       sdbus_qw_weather_object);
  SdBusProperty qw_ws_mainpid("MainPID", "u", "emits-change",
                              sdbus_systemd_service);

  value = qw_ws_mainpid.getValue();
  if (value.has_value() == false) {
    logger.log(LOG_ERR,
               fmt::format("Getting quietwind.weather.service MainPID failed"));
    return unexpected(false);
  }

  /*
   * Check that the value is a uint32_t
   */
  if (std::holds_alternative<SdBusNumericResult>(value.value()) == false) {
    logger.log(LOG_ERR,
               fmt::format("Request for MainPID did not return a uint32_t"));
    return unexpected(false);
  }

  /*
   * We know that MainPID uses signature "u". So we get the "u" field
   * from the Numeric Result.
   */
  SdBusNumericResult val = std::get<SdBusNumericResult>(value.value());
  
  return val.u;
}

bool isASystemdProcess() {

  expected<string, bool> state;
  expected<pid_t, bool> main_pid;
  pid_t my_pid;

  state = quietwindWeatherServiceState();

  if (state.has_value() == false);
    return false;

  if (state.value() != "running") {
    return false;
  }

  my_pid = getpid();

  main_pid = quietwindWeatherServiceMainPID();
  if (main_pid.has_value() == false);
    return false;

  if (main_pid.value() != my_pid) {
    return false;
  }

  return true; 
}