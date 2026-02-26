cmake_minimum_required (VERSION 3.9.2)

add_subdirectory(ThirdParty)
add_subdirectory(Resources)
add_subdirectory(Physics)
add_subdirectory(Fabrics)


option(RHEA_WITH_GRAPHICS "Enable graphics support for Rhea" ON)

add_library(RheaBase "Unit.cpp" "UnitTree.cpp" "Realm.cpp" "InputMap.cpp" "Dispatcher.cpp"  "Platform/PlatformService.cpp" 
 "EventInput.cpp" "Application.cpp" "Component.cpp"
"Core/Material.cpp" "Core/SceneLoader.cpp" "Core/Scene.cpp" "Core/Avatar.cpp" "Core/Position.cpp"
"Core/Transform.cpp" "Core/Shader.cpp" "Core/Item.cpp" 
"Core/WorldService.cpp" "Core/Lighting.cpp" "Core/Lighting.cpp" "Physics/PhysicsManagerService.cpp")
add_library(Rhea::Core ALIAS RheaBase)

target_link_libraries(RheaBase PUBLIC Meworla::Math Meworla::Orion SimpleParser RheaResources Rhea::Physics RheaFabrics  Psio)

target_include_directories(RheaBase PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# helpers for consumers
set(RHEA_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE INTERNAL "")
add_library(RheaRuntime INTERFACE)
add_library(Rhea::Runtime ALIAS RheaRuntime)

target_link_libraries(RheaRuntime INTERFACE
  Rhea::Core
)

