#
# CMakeLists.txt file for the devices
#
# Add the standard library to the build

add_library(i2cdevices STATIC
  sht4x.cpp
  # i2c_lps22hb.cpp
)

target_include_directories(i2cdevices PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(i2cdevices PUBLIC
    units
)
