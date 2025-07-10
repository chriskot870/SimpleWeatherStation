add_library(speed_units STATIC
  miles_per_hour.cpp
  kilometers_per_hour.cpp
  meters_per_second.cpp
  knots.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(speed_units PUBLIC , ${CMAKE_CURRENT_SOURCE_DIR}/include)


#
# Use the C++23 option
#
target_compile_options(speed_units PUBLIC -std=c++23)
