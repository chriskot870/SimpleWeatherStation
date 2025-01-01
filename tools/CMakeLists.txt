#
# Also build the process that will update the Weather Underground
# configuration file
#
add_executable(file_access_comparison
    file_access_comparison.cpp
    )
target_compile_options(file_access_comparison PUBLIC -std=c++23)
target_include_directories(file_access_comparison PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(file_access_comparison PRIVATE
    jsoncpp
    )
