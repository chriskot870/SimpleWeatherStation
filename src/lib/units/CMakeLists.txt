add_library(units INTERFACE
    temperature_interface.h
    relative_humidity_interface.h
    pressure_interface.h
    )

target_include_directories(units INTERFACE , ${CMAKE_CURRENT_SOURCE_DIR})