# CMakeList.txt: CMake-Projekt für "Poseidon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/Components ${CMAKE_SOURCE_DIR}/ThirdParty)

# define the main library
add_library (Poseidon "Core.cpp" "Node.cpp" "Mesh.cpp" "Viewport.cpp" "Image.cpp" 
"Texture.cpp" "Material.cpp" "Camera.cpp" "Entity.cpp" "Position.cpp" "RenderQueue.cpp" "SceneManager.cpp"
"Math/Quaternion.cpp" "Math/Vector3.cpp" "Math/Matrix3.cpp" "Math/Matrix4.cpp" "Math/Matrix3x4.cpp"
 "SceneManager.cpp" "Window.cpp" "Shader.h" "Shader.cpp" "Rectangle2D.cpp" "Rectangle2D.h" "RenderContext.h" "RenderContext.cpp")

# build the components
target_link_libraries(Poseidon Psio)

# add the renderer engine
target_link_libraries(Poseidon HippiosEngine)
