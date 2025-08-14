#
# This tests the units 
#
add_executable(units_temperature.test units_temperature_test.cpp)

target_link_libraries(units_temperature.test PRIVATE
  temperature_units
  gtest_main
  gtest
  fmt)

include_directories(
  ${CMAKE_SOURCE_DIR}/src/lib
)

target_compile_options(units_temperature.test PRIVATE -std=c++23)

#
# When the test is run it is run with current directory to the corresponding directory.
# in build. It also seems to prepend a "./" to the executable namee. So, you can just
# use the executabls name.
# When I tried to use the full path to the build directory where the executable is,
# it did not work. It seemed to prepend a "./" to the executable name.
#
# You need to have enable_testing() set in the top level CMakefile.txt for this to work.
#
add_test(NAME UnitsTemperatureTests COMMAND ${CMAKE_SOURCE_DIR}/cmake/run_remote_test.sh units_temperature.test)

