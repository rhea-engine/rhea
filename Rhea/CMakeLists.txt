cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Components/)

add_subdirectory(Physics)
add_subdirectory(Fabrics)
add_subdirectory(Components)

add_library(Rhea "Importer.cpp"  "Unit.cpp" "UnitTree.cpp" "Realm.cpp" "TextResource.cpp" "InputMap.cpp" "Dispatcher.cpp"  "Platform/PlatformService.cpp" 
 "EventInput.cpp" "ResourceLoader.h" "ResourceLoader.cpp" "ResourceList.cpp" "Error.h" "Resource.cpp" 
"Application.cpp")

target_link_libraries(Rhea PoseidonMath Orion SimpleParser RheaPhysics RheaFabrics RheaComponents Psio)

add_library(RheaClient "Controller/BasicController.cpp" "Controller/SpectatorController.cpp" 
"Graphics/GraphicsService.cpp" "Graphics/Texture.cpp" "Graphics/ItemManager.cpp" "Graphics/SceneManager.cpp" 
"Graphics/CameraManager.cpp" "Graphics/RenderElement.h" "Graphics/RenderElement.cpp"
"Graphics/GraphicsManager.h" "Graphics/MaterialManager.cpp"
"Graphics/GraphicsManager.cpp" "Graphics/MeshElement.h"  "Graphics/MeshManager.h" "Graphics/MeshManager.cpp")

target_link_libraries(RheaClient Rhea Poseidon Winmm RheaPhysicsDebug)