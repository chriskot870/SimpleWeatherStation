#
# This tests the units 
#
add_executable(units_temperature_test units_temperature_test.cpp)

target_link_libraries(units_temperature_test PRIVATE
  temperature_units
  gtest_main
  gtest
  fmt)

include_directories(
  ${CMAKE_SOURCE_DIR}/src/lib
)

target_compile_options(units_temperature_test PRIVATE -std=c++23)

add_custom_command(
    TARGET units_temperature_test
    POST_BUILD
    COMMAND ln -sf units_temperature_test a.out
    COMMENT "Making sym link to units_pressure_test for easier debugging"
   )

add_test(
  NAME UnitsTemperatureTests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  #COMMAND ${CMAKE_SOURCE_DIR}/cmake/run_remote_test.sh units_temperature_test
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/units_temperature_test
  --gtest_output=json:units_temperature_test_results.json
  --gtest_filter=UnitsTemperatureTest.*)

#add_custom_command(
#  OUTPUT a.out
#  COMMAND ln -sf units_temperature_test a.out
#  DEPENDS units_temperature_test
#  COMMENT "Creating a hard link to units_temperature_test for easier debugging"
#  )