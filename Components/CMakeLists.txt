cmake_minimum_required(VERSION 3.13)

project(RheaCommon VERSION 0.9.0 LANGUAGES C CXX)

if (TARGET Components)
    return()
endif()

add_subdirectory(ThirdParty/stb_image)

add_subdirectory(SimpleParser)
add_subdirectory(Math)

if(PROJECT_IS_TOP_LEVEL)

    if (NOT TARGET Meworla::Psio)
        add_subdirectory(Psio)
    endif()

    if(NOT TARGET Psio)

        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Psio/CMakeLists.txt")
            add_subdirectory(
                ${CMAKE_CURRENT_SOURCE_DIR}/../Psio
                ${CMAKE_CURRENT_BINARY_DIR}/_deps/Psio-build
            )
        else()
            include(FetchContent)

            FetchContent_Declare(
                Psio
                GIT_REPOSITORY https://github.com/rhea-engine/psio.git
                GIT_TAG v0.1.0
            )

            FetchContent_MakeAvailable(Psio)
        endif()
    endif()
endif()

add_library(Components INTERFACE)
add_library(Meworla::Components ALIAS Components)
target_include_directories(Components INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Components INTERFACE Meworla::Psio Meworla::SimpleParser Meworla::Math)