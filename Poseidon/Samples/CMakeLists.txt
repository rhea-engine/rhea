# Poseidon Samples - Low-Level Renderer API Examples

if(NOT POSEIDON_BUILD_SAMPLES)
    return()
endif()

message(STATUS "Building Poseidon Samples (Low-Level Renderer)")

# Shared Sample Base Class
add_library(PoseidonSampleBase STATIC
    PoseidonApp.cpp
    PoseidonApp.h
)

target_link_libraries(PoseidonSampleBase PUBLIC
    Meworla::Poseidon
    Meworla::Psio
    Meworla::Hippios
)

target_include_directories(PoseidonSampleBase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(PoseidonSampleBase PROPERTIES
    FOLDER "Samples/Poseidon"
)

# Sample 1: Triangle
add_executable(Poseidon_Sample_01_Triangle
    01_Triangle/Triangle.cpp
)

target_link_libraries(Poseidon_Sample_01_Triangle PRIVATE
    PoseidonSampleBase
)

set_target_properties(Poseidon_Sample_01_Triangle PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    FOLDER "Samples/Poseidon"
)

# Sample 2: Cube
add_executable(Poseidon_Sample_02_Cube
    02_Cube/Cube.cpp
)

target_link_libraries(Poseidon_Sample_02_Cube PRIVATE
    PoseidonSampleBase
)

set_target_properties(Poseidon_Sample_02_Cube PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    FOLDER "Samples/Poseidon"
)

# Sample 3: Textured Cube
add_executable(Poseidon_Sample_03_TexturedCube
    03_TexturedCube/TexturedCube.cpp
)

target_link_libraries(Poseidon_Sample_03_TexturedCube PRIVATE
    PoseidonSampleBase
)

set_target_properties(Poseidon_Sample_03_TexturedCube PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    FOLDER "Samples/Poseidon"
)