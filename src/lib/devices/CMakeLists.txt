#
# Add the i2c sub-directory
#

#
# Add any sub-directories that need to have
# something built
#
set(CMAKE_VERBOSE_MAKEFILE ON)

add_library(weatherdevices STATIC
  anomometer_adafruit.cpp
)

# add_compile_options(-std=c++23) to use expected class
target_compile_options(weatherdevices PUBLIC -std=c++23)

target_include_directories(weatherdevices PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c/include
)

target_link_libraries(weatherdevices PUBLIC
  speed_units
)

add_subdirectory(i2c)
