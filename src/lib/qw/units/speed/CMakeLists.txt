add_library(speed_units STATIC
  speed.cpp
  miles_per_hour.cpp
  kilometers_per_hour.cpp
  meters_per_second.cpp
  knots.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(speed_units PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)


#
# Use the C++23 option
#
target_compile_options(speed_units PUBLIC -std=c++23)
