cmake_minimum_required(VERSION 3.18)

add_subdirectory(Lobby)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ )

add_library(MeworlaFabrics "meworlafabrics.cpp" "meworlafabrics.h")

target_link_libraries(MeworlaFabrics Lobby)