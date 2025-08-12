add_library(light_units STATIC
  lux.cpp
  foot_candle.cpp
  light.cpp
  light_measurement.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(light_units PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

#
# Use the C++23 option
#
target_compile_options(light_units PUBLIC -std=c++23)