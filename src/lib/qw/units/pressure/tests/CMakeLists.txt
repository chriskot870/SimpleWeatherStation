#
# This tests the units 
#
#set(CMAKE_CROSSCOMPILING_EMULATOR
#    ${CMAKE_SOURCE_DIR}/cmake/run_remote.sh
#    CACHE STRING "Command to run executables on target")

add_executable(units_pressure_test units_pressure_test.cpp)

target_link_libraries(units_pressure_test PRIVATE
  pressure_units
  gtest_main
  gtest
  fmt)

include_directories(
  ${CMAKE_SOURCE_DIR}/src/lib
)

target_compile_options(units_pressure_test PRIVATE -std=c++23)

#add_custom_command(
#    TARGET units_pressure_test
#    POST_BUILD
#    COMMAND ln -sf units_pressure_test a.out
#    COMMENT "Making hard link to units_pressure_test for easier debugging"
#   )

set_property(TARGET units_pressure_test PROPERTY TEST_LAUNCHER
  ${CMAKE_SOURCE_DIR}/cmake/run_test_remotely.sh
  )

add_test(
  NAME DebugUnitsPressureTests
  CONFIGURATIONS Debug Release
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/units_pressure_test
  --gtest_output=json:units_pressure_test_results.json
  --gtest_filter=UnitsPressureTest.*
  )

add_test(
  NAME UnitsPressureTests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${CMAKE_SOURCE_DIR}/cmake/run_remote_test.sh
    units_pressure_test
    --gtest_output=json:units_pressure_test_results.json
    --gtest_filter=UnitsPressureTest.*
  )


#add_custom_command(
#  OUTPUT a.out
#  COMMAND ln -sf units_pressure_test a.out
#  DEPENDS units_pressure_test
#  COMMENT "Creating a sym link to units_pressure_test for easier debugging"
#  )

