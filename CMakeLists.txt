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
add_subdirectory(tools)

#
# The include(Cpack) creates a default package build. In order to control
# the CPACK settings you need to define a CPACK_PROPERTIES_FILE prior to
# including CPack. WHen you include CPack the CMake system creates a
# CPackConfig.cmake file in the build directory. These have default settings.
# At the end of that file there is an include(${CPACK+PROPERTIES_FILES}).
# This allows you to change any default setting and add others.
# It doesn't seem like there needs to be a Cpack.cmkae file at the top
# level. Just having include(CPcak) seem to be adequate.
# Be sure and set PACK_PROPERTIES_FILE pror to including CPack.
#
# After a build you can cd to build and run cpack and it should build
# a packge for you.
#
# set(CPACK_PROPERTIES_FILE ${CMAKE_SOURCE_DIR}/cmake/cpack_debian.cmake)
#set(CPACK_GENERATOR "DEB")
include(${CMAKE_SOURCE_DIR}/cmake/cpack_debian.cmake)
include(CPack)



