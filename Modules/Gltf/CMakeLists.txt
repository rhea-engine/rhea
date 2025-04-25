include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (RheaGltf
"include/Rhea/Gltf/GlbLoader.h" 
"include/Rhea/Gltf/GltfLoader.h" 
"src/GltfLoader.cpp" 
"include/Rhea/Gltf/GltfFabrics.h" 
"src/GltfFabrics.cpp" 
"include/Rhea/Gltf/GltfMesh.h" 
"src/GltfMesh.cpp" 
"include/Rhea/Gltf/GltfScene.h" 
"src/GltfScene.cpp")

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaGltf PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaGltf PUBLIC RheaEngine)