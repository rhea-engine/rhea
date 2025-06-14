include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (RheaCharacter
"include/Rhea/Character/CharacterFabrics.h" 
"src/CharacterFabrics.cpp" 
"include/Rhea/Character/Character.h" 
"src/Character.cpp" 
"include/Rhea/Character/KinematicCharacter.h" 
"src/KinematicCharacter.cpp" 
)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaCharacter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaCharacter PUBLIC RheaEngine)