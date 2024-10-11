#
# This is the CmakeLists.txt file for the source.
# We want to build a single executable with
# the same name as the project.
# Set the executable name to the same as the project name.
#
set(EXECUTABLE_NAME ${CMAKE_PROJECT_NAME})

#
# Add the executable to be built and the local files used
# to build the executable
#
add_executable(${EXECUTABLE_NAME}
               main.cpp
)

target_compile_options(${EXECUTABLE_NAME} PUBLIC -std=c++23)

#
# List the sub-directories that need to be built
#
add_subdirectory(lib)

#
# List the libraries that will be linked to this executable.
# Note we don't need the full path to the libraries from here.
# Just the library name.
#
target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    i2cdevices
    units)