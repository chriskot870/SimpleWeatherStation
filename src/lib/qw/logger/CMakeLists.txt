add_library(logger STATIC
  logger.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(logger PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

#
# Use the C++23 option
#
target_compile_options(logger PUBLIC -std=c++23)

#target_link_libraries(system_utilities PUBLIC
#  systemd
#)
