add_library(systemd_objects STATIC
  sd_manager_obj.cpp
  sd_unit_obj.cpp
  sd_service_unit_obj.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(systemd_objects PUBLIC , ${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Use the C++23 option
#
target_compile_options(systemd_objects PUBLIC -std=c++23)

target_link_libraries(systemd_objects PUBLIC
  systemd
)