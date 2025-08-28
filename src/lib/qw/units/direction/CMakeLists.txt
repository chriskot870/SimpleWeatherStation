add_library(direction_units STATIC
  direction.cpp
  degrees.cpp
  radians.cpp
  compass_heading.cpp
  direction_measurement.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(direction_units PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

#
# Use the C++23 option
#
target_compile_options(direction_units PUBLIC -std=c++23)

#add_subdirectory(tests)