add_library(humidity_units STATIC
    relative_humidity.cpp
    )

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(humidity_units PUBLIC , ${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Use the C++23 option
#
target_compile_options(humidity_units PUBLIC -std=c++23)
