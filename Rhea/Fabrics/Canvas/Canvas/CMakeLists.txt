
add_library(Canvas)

target_sources(Canvas PRIVATE
    src/Canvas.cpp
)

target_include_directories(Canvas PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
