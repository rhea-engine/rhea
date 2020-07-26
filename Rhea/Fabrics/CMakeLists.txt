cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Rhea/)

add_library (RheaFabrics "Character/Character.cpp" "Character/KinematicCharacter.cpp" "Terrain/Terrain.cpp" "Canvas/Item2D.cpp")