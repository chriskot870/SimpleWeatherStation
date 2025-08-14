add_library(temperature_units STATIC
  temperature.cpp
  celsius.cpp
  fahrenheit.cpp
  kelvin.cpp
  temperature_measurement.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(temperature_units PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

#
# Use the C++23 option
#
target_compile_options(temperature_units PUBLIC -std=c++23)

add_subdirectory(tests)
