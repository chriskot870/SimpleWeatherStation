#
# This is the CmakeLists.txt file for the source.
# We want to build a single executable with
# the same name as the project.
# Set the executable name to the same as the project name.
#
set(EXECUTABLE_NAME ${CMAKE_PROJECT_NAME})

find_package(fmt REQUIRED)
#set(jsoncpp_DIR /usr/lib/x86_64-linux-gnu/cmake/jsoncpp)
#find_package(jsoncpp REQUIRED)
#
# Add the executable to be built and the local files used
# to build the executable
#
add_executable(${EXECUTABLE_NAME}
               main.cpp
)

target_compile_options(${EXECUTABLE_NAME} PUBLIC -std=c++23)
#target_compile_options(${EXECUTABLE_NAME} PUBLIC -lfmt)

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Add the weather_station library
#
add_library(weatherstation STATIC
  weather_station_config.cpp
)

# What are weatherstation library compile options
# add_compile_options(-std=c++23) to use expected class
target_compile_options(weatherstation PUBLIC -std=c++23)

# What are weatherstation library include paths
target_include_directories(weatherstation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Weather libraries do weatherstation need to link with
target_link_libraries(weatherstation PRIVATE
  system_utilities
)

#
# Add systemdqwweather library
#
add_library(systemdqwweather STATIC
  systemd_quietwind_weather.cpp
)

# What are weatherstation library compile options
# add_compile_options(-std=c++23) to use expected class
target_compile_options(systemdqwweather PUBLIC -std=c++23)

# What are weatherstation library include paths
target_include_directories(systemdqwweather PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Weather libraries do weatherstation need to link with
target_link_libraries(systemdqwweather PRIVATE
  system_utilities
)

#
# List the libraries that will be linked to this executable.
# Note we don't need the full path to the libraries from here.
# Just the library name.
#
target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    weatherstation
    systemdqwweather
    i2cdevices
    weatherunderground
    temperature_units
    pressure_units
    humidity_units
    weather_utilities
    system_utilities
    fmt
    curl
    systemd
    jsoncpp
    )

#
# define where to install the targets with respect to CMAKE_INSTALL_PREFIX
# 
install(TARGETS WeatherStation DESTINATION bin)
install(FILES config/wu_access.json DESTINATION etc)
install(FILES config/quietwind.weather.service DESTINATION etc/systemd/system)

#
# List the sub-directories that need to be built
#
add_subdirectory(lib)