# define the main library
add_library (Poseidon STATIC "Core.cpp" "Node.cpp" "Mesh.cpp" "Viewport.cpp" 
"Texture.cpp" "Material.cpp" "Camera.cpp" "Entity.cpp" "RenderQueue.cpp" "SceneManager.cpp"
 "SceneManager.cpp" "Window.cpp" "Shader.h" "Shader.cpp" "RenderContext.h" "RenderContext.cpp" "Poseidon.cpp" "Cubemap.h" "Cubemap.cpp")
add_library(Meworla::Poseidon ALIAS Poseidon)

target_include_directories(Poseidon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link Poseidon’s real dependencies as targets
target_link_libraries(Poseidon
  PUBLIC
    Meworla::Components
    Meworla::Hippios
    Rhea::Client
)
target_compile_options(Poseidon PRIVATE /showIncludes)

target_include_directories(Poseidon PRIVATE
  $<TARGET_PROPERTY:Meworla::HippiosHeaders,INTERFACE_INCLUDE_DIRECTORIES>
)

get_target_property(_hip_inc Meworla::HippiosHeaders INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "Hippios include dirs seen by consumers: ${_hip_inc}")
