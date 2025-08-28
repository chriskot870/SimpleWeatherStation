add_library(weather STATIC
  dewpoint.cpp
  #windspeed_history.cpp
  thermometer.cpp
  barometer.cpp
  hygrometer.cpp
  anemometer.cpp
  wind_vane.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(weather PUBLIC , ${CMAKE_SOURCE_DIR}/src/lib)

#
# Use the C++23 option
#
target_compile_options(weather PUBLIC -std=c++23)

target_link_libraries(weather PUBLIC
  temperature_units
  humidity_units
  pressure_units
  speed_units
  direction_units
)