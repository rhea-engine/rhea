cmake_minimum_required(VERSION 3.21)

project(MeworlaComponents VERSION 0.1.0 LANGUAGES C CXX)

if (TARGET Meworla::Components)
    return()
endif()

add_subdirectory(ThirdParty/stb_image)

add_subdirectory(SimpleParser)
add_subdirectory(Math)

if (NOT TARGET Meworla::Psio AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Psio/CMakeLists.txt")
    add_subdirectory(Psio)
endif()

add_library(MeworlaComponents INTERFACE)
add_library(Meworla::Components ALIAS MeworlaComponents)
target_include_directories(MeworlaComponents
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(MeworlaComponents INTERFACE Meworla::Psio Meworla::SimpleParser Meworla::Math)