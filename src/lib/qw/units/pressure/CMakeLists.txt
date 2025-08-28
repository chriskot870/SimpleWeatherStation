add_library(pressure_units STATIC
  pressure.cpp
  inches_mercury.cpp
  millibar.cpp
  pressure_measurement.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(pressure_units PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

#
# Use the C++23 option
#
target_compile_options(pressure_units PUBLIC -std=c++23)

# Add the test directory
add_subdirectory(tests)
