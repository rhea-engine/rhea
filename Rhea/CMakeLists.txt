add_subdirectory(ThirdParty)
add_subdirectory(Resources)
add_subdirectory(Fabrics)


option(RHEA_WITH_GRAPHICS "Enable graphics support for Rhea" ON)

add_library(RheaBase "src/Unit.cpp" "src/UnitTree.cpp" "src/Realm.cpp" "src/InputMap.cpp" "src/Dispatcher.cpp"  "Platform/PlatformService.cpp" 
 "src/EventInput.cpp" "src/Application.cpp" "src/Component.cpp"
"Core/Material.cpp" "Core/SceneLoader.cpp" "Core/Scene.cpp" "Core/Avatar.cpp" "Core/Position.cpp"
"Core/Transform.cpp" "Core/Shader.cpp" "Core/Item.cpp" "Core/Item2D.cpp"
"Core/WorldService.cpp" "Core/Lighting.cpp" "Core/Lighting.cpp" 
"Core/Camera.cpp" "Core/Mesh.cpp" 
"Core/Light.cpp" "Core/DirectionalLight.cpp" "Core/PointLight.cpp" 
"Core/SpotLight.cpp"
"Physics/PhysicsManagerService.cpp" "include/Rhea/Graphics/SizeEvent.h" "include/Rhea/ConfigFile.h" "src/ConfigFile.cpp")
add_library(Rhea::Core ALIAS RheaBase)

target_link_libraries(RheaBase PUBLIC Meworla::Math Meworla::Orion SimpleParser RheaResources Psio)

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

