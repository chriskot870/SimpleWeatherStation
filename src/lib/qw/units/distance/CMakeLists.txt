add_library(distance_units STATIC
  millimeter.cpp
  inches.cpp
  distance.cpp
  distance_measurement.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(distance_units PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

#
# Use the C++23 option
#
target_compile_options(distance_units PUBLIC -std=c++23)
