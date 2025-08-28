add_library(uvi_units STATIC
    uvi.cpp
    )

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(uvi_units PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

#
# Use the C++23 option
#
target_compile_options(uvi_units PUBLIC -std=c++23)
