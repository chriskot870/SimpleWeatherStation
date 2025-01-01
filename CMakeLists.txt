#
# This is the top-level CmakeLists.txt file
#
cmake_minimum_required(VERSION 3.20.0)

#
# Need to set this variable prior to project
#
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain.cmake)

# set the prefix to /usr/local/qw
# TARGETS will get installed into /usr/local/qw/bin by default
# This needs to be done prior to defining the project
set(CMAKE_INSTALL_PREFIX  "/usr/local/qw")

#
# Set the project name and the compilers to be used
#
project(WeatherStation CXX)

#
# Add the sub-directories to build
#
add_subdirectory(src)
