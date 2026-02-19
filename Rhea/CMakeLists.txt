cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Components/)

add_subdirectory(ThirdParty)
add_subdirectory(Resources)
add_subdirectory(Physics)
add_subdirectory(Fabrics)


option(RHEA_WITH_GRAPHICS "Enable graphics support for Rhea" ON)

add_library(RheaBase "Unit.cpp" "UnitTree.cpp" "Realm.cpp" "InputMap.cpp" "Dispatcher.cpp"  "Platform/PlatformService.cpp" 
 "EventInput.cpp" "Error.h" "Application.cpp" "Component.h" "Component.cpp"
"Core/Material.h" "Core/Material.cpp" "Core/SceneLoader.cpp" "Core/Scene.cpp" "Core/Avatar.h" "Core/Avatar.cpp" "Core/Position.h" "Core/Position.cpp"
"Core/Transform.h" "Core/Transform.cpp" "Core/Geometry.h" "Core/Shader.h"  "Core/Shader.cpp" "Core/Item.h" "Core/Item.cpp" "Core/Controller.h"
"Core/WorldService.h" "Core/WorldService.cpp" "Core/Lighting.cpp" "Core/Lighting.h" "Core/Lighting.cpp" "ModuleSystem.h")
add_library(Rhea::Core ALIAS RheaBase)

target_link_libraries(RheaBase PUBLIC PoseidonMath Meworla::Orion SimpleParser RheaResources RheaPhysics RheaFabrics  Psio)

# helpers for consumers
set(RHEA_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE INTERNAL "")
add_library(RheaRuntime INTERFACE)
add_library(Rhea::Runtime ALIAS RheaRuntime)

target_link_libraries(RheaRuntime INTERFACE
  Rhea::Core
)

