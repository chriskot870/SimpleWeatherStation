add_library(temperature_units STATIC
    temperature.cpp
    celsius.cpp
    fahrenheit.cpp
    kelvin.cpp
    )

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(temperature_units PUBLIC , ${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Use the C++23 option
#
target_compile_options(temperature_units PUBLIC -std=c++23)
