#
# Add the units library. This library is an interface
# library that just contains header files
#
add_library(unit_interfaces INTERFACE
    temperature_interface.h
    relative_humidity_interface.h
    pressure_interface.h
    )

add_library(units STATIC
    temperature_datum.cpp
    pressure_datum.cpp
    relative_humidity_datum.cpp
    temperature_measurement.cpp
    pressure_measurement.cpp
    relative_humidity_measurement.cpp
    dew_point.cpp
    )

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(unit_interfaces INTERFACE , ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(units PUBLIC , ${CMAKE_CURRENT_SOURCE_DIR})

#
# Use the C++23 option
#
target_compile_options(unit_interfaces INTERFACE -std=c++23)
target_compile_options(units PUBLIC -std=c++23)
