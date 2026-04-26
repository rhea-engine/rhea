# define the main library
add_library (Poseidon STATIC "src/Core.cpp" "src/Mesh.cpp" "src/Viewport.cpp" 
"src/Texture.cpp" "src/Material.cpp" "src/Camera.cpp" "src/RenderQueue.cpp" "src/FrameRenderer.cpp"
"src/Window.cpp" "src/Shader.cpp" "src/RenderContext.cpp" "src/Cubemap.cpp" "src/RenderWorld.cpp")

add_library(Meworla::Poseidon ALIAS Poseidon)

target_include_directories(Poseidon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link Poseidon’s real dependencies as targets
target_link_libraries(Poseidon
  PUBLIC
    Meworla::Components
    Meworla::Hippios
)
if(WIN32)
    target_compile_options(Poseidon PRIVATE /showIncludes)
endif()

target_include_directories(Poseidon PRIVATE
  $<TARGET_PROPERTY:Meworla::HippiosHeaders,INTERFACE_INCLUDE_DIRECTORIES>
)

# Samples (optional)
if(POSEIDON_BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()

