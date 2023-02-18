# CMakeList.txt: CMake-Projekt für "Poseidon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/Components ${CMAKE_SOURCE_DIR}/ThirdParty ${CMAKE_SOURCE_DIR}/ThirdParty/bullet/src)

# define the main library
add_library (Poseidon "Core.cpp" "Node.cpp" "Mesh.cpp" "Viewport.cpp" 
"Texture.cpp" "Material.cpp" "Camera.cpp" "Entity.cpp" "RenderQueue.cpp" "SceneManager.cpp"
 "SceneManager.cpp" "Window.cpp" "Shader.h" "Shader.cpp" "RenderContext.h" "RenderContext.cpp" "Poseidon.cpp"
"Network/ClientService.h" "Network/ClientService.cpp" "Network/WorldClient.h" "Network/WorldClient.cpp")

# build the components
#target_link_libraries(Poseidon Psio)


# add the renderer engine
target_link_libraries(Poseidon HippiosEngine Hermes)
