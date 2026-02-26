# define the main library
add_library (Poseidon STATIC "src/Core.cpp" "src/Node.cpp" "src/Mesh.cpp" "src/Viewport.cpp" 
"src/Texture.cpp" "src/Material.cpp" "src/Camera.cpp" "src/Entity.cpp" "src/RenderQueue.cpp" "src/SceneManager.cpp"
 "src/SceneManager.cpp" "src/Window.cpp" "src/Shader.cpp" "src/RenderContext.cpp" "src/Poseidon.cpp" "src/Cubemap.cpp")
add_library(Meworla::Poseidon ALIAS Poseidon)

target_include_directories(Poseidon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

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
