include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (RheaTerrain
"include/Rhea/Terrain/TerrainFabrics.h" 
"src/TerrainFabrics.cpp" 
"include/Rhea/Terrain/Terrain.h" 
"src/Terrain.cpp"
"include/Rhea/Terrain/BulletHeightMap.h"
"src/BulletHeightMap.cpp"
)
add_library(Rhea::Terrain ALIAS RheaTerrain)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaTerrain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaTerrain PUBLIC RheaEngine)