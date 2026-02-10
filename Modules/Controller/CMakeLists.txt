include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (RheaController
"include/Rhea/Controller/ControllerFabrics.h" 
"src/ControllerFabrics.cpp" 
"include/Rhea/Controller/BasicController.h" 
"src/BasicController.cpp" 
"include/Rhea/Controller/SpectatorController.h" 
"src/SpectatorController.cpp" 
)
add_library(Rhea::Controller ALIAS RheaController)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaController PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaController PUBLIC RheaEngine)