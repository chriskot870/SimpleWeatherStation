add_library(system_utilities STATIC
  locking_file.cpp
  logger.cpp
  systemd.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(system_utilities PUBLIC , ${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Use the C++23 option
#
target_compile_options(system_utilities PUBLIC -std=c++23)

target_link_libraries(system_utilities PUBLIC
  systemd
)

add_subdirectory(systemd)
