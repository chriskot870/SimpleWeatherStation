#
# This is the top-level CmakeLists.txt file
#
cmake_minimum_required(VERSION 3.20.0)

#
# Need to set this variable prior to project
#
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain.cmake)

#
# Set the project name and the compilers to be used
#
project(WeatherStation CXX)

#
# Add the sub-directories to build
#
add_subdirectory(src)
