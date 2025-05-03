cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Components/)

add_subdirectory(Resources)
add_subdirectory(Physics)
add_subdirectory(Fabrics)
add_subdirectory(Components)

option(RHEA_WITH_GRAPHICS "Enable graphics support for Rhea" ON)

add_library(RheaBase "Unit.cpp" "UnitTree.cpp" "Realm.cpp" "InputMap.cpp" "Dispatcher.cpp"  "Platform/PlatformService.cpp" 
 "EventInput.cpp" "Error.h" "Application.cpp" "Component.h" "Component.cpp"
"Core/Material.h" "Core/Material.cpp" "Core/SceneLoader.cpp" "Core/Scene.cpp" "Core/Avatar.h" "Core/Avatar.cpp" "Core/Position.h" "Core/Position.cpp"
"Core/Transform.h" "Core/Transform.cpp" "Core/Geometry.h" "Core/Shader.h"  "Core/Shader.cpp" "Core/Item.h" "Core/Item.cpp" 
"Core/WorldService.h" "Core/WorldService.cpp" "Core/Lighting.cpp" "Core/Lighting.h" "Core/Lighting.cpp" "ModuleSystem.h")

target_link_libraries(RheaBase PoseidonMath Orion SimpleParser RheaResources RheaPhysics RheaFabrics RheaComponents Psio)

if(RHEA_WITH_GRAPHICS)
	add_library(RheaClient
	"Graphics/GraphicsService.cpp" "Graphics/Texture.cpp" "Graphics/ItemManager.cpp" "Graphics/SceneManager.cpp" 
	"Graphics/CameraManager.cpp" "Graphics/RenderElement.h" "Graphics/RenderElement.cpp"
	"Graphics/GraphicsManager.h" "Graphics/MaterialManager.cpp"
	"Graphics/GraphicsManager.cpp" "Graphics/MeshElement.h"  "Graphics/MeshManager.h" "Graphics/MeshManager.cpp" "Graphics/GraphicsComponent.h" "Graphics/GraphicsElement.h")

	target_link_libraries(RheaClient RheaBase Poseidon Winmm RheaPhysicsDebug)

	add_library(Rhea ALIAS RheaClient)
else()
	add_library(Rhea ALIAS RheaBase)
endif()
