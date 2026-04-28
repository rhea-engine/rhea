cmake_minimum_required(VERSION 3.20)

add_library(ogg STATIC
    src/bitwise.c
    src/framing.c
)

target_include_directories(ogg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(ogg PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)