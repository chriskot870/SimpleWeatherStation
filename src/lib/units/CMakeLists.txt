#
# Add the units library. This library is an interface
# library that just contains header files
#
add_library(units INTERFACE
    temperature_interface.h
    relative_humidity_interface.h
    pressure_interface.h
    )

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(units INTERFACE , ${CMAKE_CURRENT_SOURCE_DIR})

#
# Use the C++23 option
#
target_compile_options(units INTERFACE -std=c++23)