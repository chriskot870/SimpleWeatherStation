#
# CMakeLists.txt file for the devices
#
# Add the standard library to the build

add_library(i2cdevices STATIC
  i2cbus.cpp
  sht4x.cpp
  lps22.cpp
)

# add_compile_options(-std=c++23) to use expected class
target_compile_options(i2cdevices PUBLIC -std=c++23)

target_include_directories(i2cdevices PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(i2cdevices PUBLIC
    units
)
