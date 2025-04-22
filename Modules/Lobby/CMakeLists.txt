cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (Lobby "Lobby.h" "Lobby.cpp" "ClientConnect.h" "ClientConnect.cpp" "Avatars.h" "Avatars.cpp" "CredentialsService.h" "CredentialsService.cpp")