cmake_minimum_required(VERSION 3.22)

project(Poseidon VERSION 0.9.0 LANGUAGES C CXX)

option(POSEIDON_BUILD_SAMPLES "Build Poseidon samples" ${PROJECT_IS_TOP_LEVEL})

# Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT TARGET Meworla::Hippios AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Hippios/CMakeLists.txt")
    add_subdirectory(Hippios)
endif()

# define the main library
add_library (Poseidon STATIC "src/Core.cpp" "src/Mesh.cpp" "src/Viewport.cpp" 
"src/Texture.cpp" "src/Material.cpp" "src/Camera.cpp" "src/RenderQueue.cpp" "src/FrameRenderer.cpp"
"src/Window.cpp" "src/Shader.cpp" "src/RenderContext.cpp" "src/Cubemap.cpp" "src/RenderWorld.cpp")

add_library(Meworla::Poseidon ALIAS Poseidon)

target_include_directories(Poseidon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(NOT TARGET Meworla::Components)
    find_package(Components CONFIG QUIET)
endif()

if(NOT TARGET Meworla::Components)  
    include(FetchContent)

    FetchContent_Declare(
        Components
        GIT_REPOSITORY https://github.com/rhea-engine/components.git
        GIT_TAG v0.9.0
    )
    
    FetchContent_MakeAvailable(Components)
endif()

# Link Poseidon’s real dependencies as targets
target_link_libraries(Poseidon
  PUBLIC
    Meworla::Components
    Meworla::Hippios
)
target_include_directories(Poseidon PRIVATE
  $<TARGET_PROPERTY:Meworla::HippiosHeaders,INTERFACE_INCLUDE_DIRECTORIES>
)

# Samples (optional)
if(POSEIDON_BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()

