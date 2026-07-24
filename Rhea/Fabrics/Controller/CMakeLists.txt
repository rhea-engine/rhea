add_library (RheaController
"include/Rhea/Controller/ControllerFabric.h" 
"src/ControllerFabric.cpp" 
"include/Rhea/Controller/BasicController.h" 
"src/BasicController.cpp" 
"include/Rhea/Controller/SpectatorController.h" 
"src/SpectatorController.cpp" 
 "include/Rhea/Controller/ControllerService.h" "src/ControllerService.cpp")
add_library(Rhea::Controller ALIAS RheaController)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaController PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaController PUBLIC Rhea::Core)

# register for auto registry generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")