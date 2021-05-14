cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Components/)

add_subdirectory(Physics)
add_subdirectory(Fabrics)

add_library (Rhea "Application.cpp" "Importer.cpp" "SceneLoader.cpp" "Unit.cpp" "UnitTree.cpp" "Realm.cpp" "Item.cpp" 
"Scene.cpp" "Mesh.cpp" "TextResource.cpp" "ImageResource.cpp" "InputMap.cpp" "Dispatcher.cpp" "Controller/BasicController.cpp"
"Controller/SpectatorController.cpp" "Platform/PlatformService.cpp" "Graphics/GraphicsService.cpp" "Graphics/Texture.cpp" 
"Graphics/ItemManager.cpp" "Graphics/SceneManager.cpp" "Graphics/CameraManager.cpp" "Camera.cpp" "Material.h" "Material.cpp" 
"Graphics/Item2DManager.cpp" "ResourceList.cpp" "Error.h" "Resource.cpp" "Graphics/RenderElement.h" "Graphics/RenderElement.cpp"
"Graphics/GraphicsManager.h" "Graphics/TextManager.h" "Graphics/TextManager.cpp" "Graphics/MaterialManager.cpp" "Graphics/GraphicsManager.cpp" 
"Graphics/MeshElement.h" "EventUnit.h" "EventInput.cpp")

target_link_libraries(Rhea Poseidon Orion SimpleParser Winmm RheaPhysics RheaFabrics)