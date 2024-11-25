add_library(weatherunderground STATIC
  weather_underground.cpp
)

# add_compile_options(-std=c++23) to use expected class
target_compile_options(weatherunderground PUBLIC -std=c++23)

target_include_directories(weatherunderground PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)