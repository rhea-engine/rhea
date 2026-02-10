cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (Lobby 
"include/Rhea/Lobby/Lobby.h" 
"src/Lobby.cpp" 
"include/Rhea/Lobby/ClientConnect.h" 
"src/ClientConnect.cpp" 
"include/Rhea/Lobby/Avatars.h" 
"src/Avatars.cpp" 
"include/Rhea/Lobby/CredentialsService.h" 
"src/CredentialsService.cpp")
add_library(Rhea::Lobby ALIAS Lobby)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(Lobby PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Lobby PUBLIC RheaEngine)