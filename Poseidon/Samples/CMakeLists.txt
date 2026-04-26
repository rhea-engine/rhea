# Poseidon Samples - Low-Level Renderer API Examples

if(NOT POSEIDON_BUILD_SAMPLES)
    return()
endif()

message(STATUS "Building Poseidon Samples (Low-Level Renderer)")

# Sample 1: Triangle - Minimal rendering
add_executable(Poseidon_Sample_01_Triangle
    01_Triangle/Triangle.cpp
    01_Triangle/Triangle.h
)

target_link_libraries(Poseidon_Sample_01_Triangle PRIVATE
    Meworla::Poseidon
)

set_target_properties(Poseidon_Sample_01_Triangle PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Sample 2: Cube - 3D mesh with basic lighting
add_executable(Poseidon_Sample_02_Cube
    02_Cube/Cube.cpp
    02_Cube/Cube.h
)

target_link_libraries(Poseidon_Sample_02_Cube PRIVATE
    Meworla::Poseidon
)

set_target_properties(Poseidon_Sample_02_Cube PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Sample 3: Textured Cube
add_executable(Poseidon_Sample_03_TexturedCube
    03_TexturedCube/TriangleTexture.cpp
    03_TexturedCube/TriangleTexture.h
)

target_link_libraries(Poseidon_Sample_03_TexturedCube PRIVATE
    Meworla::Poseidon
)

set_target_properties(Poseidon_Sample_03_TexturedCube PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)