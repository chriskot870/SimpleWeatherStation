

add_library(weather_utilities STATIC
  dewpoint.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(weather_utilities PUBLIC , ${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Use the C++23 option
#
target_compile_options(weather_utilities PUBLIC -std=c++23)

target_link_libraries(weather_utilities PUBLIC
  temperature_units
  humidity_units
)
