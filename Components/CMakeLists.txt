cmake_minimum_required(VERSION 3.13)

project(RheaCommon VERSION 0.9.0 LANGUAGES C CXX)

if (TARGET Components)
    return()
endif()

add_subdirectory(ThirdParty/stb_image)

add_subdirectory(SimpleParser)
add_subdirectory(Math)

if (NOT TARGET Meworla::Psio AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Psio/CMakeLists.txt")
    add_subdirectory(Psio)
endif()

add_library(Components INTERFACE)
add_library(Meworla::Components ALIAS Components)
target_include_directories(Components INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Components INTERFACE Meworla::Psio Meworla::SimpleParser Meworla::Math)