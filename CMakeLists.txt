#
# This is the top-level CmakeLists.txt file
#
cmake_minimum_required(VERSION 3.20.0)

#
# Need to set this variable prior to project
#
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain.cmake)

project(WeatherStation CXX)

add_subdirectory(src)
