cmake_minimum_required(VERSION 3.18)

add_subdirectory(Lobby)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/Components)

add_library(MeworlaFabrics "meworlafabrics.cpp" "meworlafabrics.h")

target_link_libraries(MeworlaFabrics Lobby)