cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (RheaLobby 
"include/Rhea/Lobby/LobbyFabrics.h" 
"src/LobbyFabrics.cpp" 
"include/Rhea/Lobby/ClientConnect.h" 
"src/ClientConnect.cpp" 
"include/Rhea/Lobby/Avatars.h" 
"src/Avatars.cpp" 
"include/Rhea/Lobby/CredentialsService.h" 
"src/CredentialsService.cpp")
add_library(Rhea::Lobby ALIAS RheaLobby)

# Scoped includes are under include/Rhea/Lobby/**
target_include_directories(RheaLobby PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaLobby PRIVATE Rhea::GUI)
target_link_libraries(RheaLobby PUBLIC RheaEngine)