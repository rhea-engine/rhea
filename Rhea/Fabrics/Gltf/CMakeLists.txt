include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (RheaFabricsGltf
"GlbLoader.h" "GltfLoader.h" "GltfLoader.cpp" "GltfFabrics.h" "GltfFabrics.cpp" "GltfMesh.h" "GltfMesh.cpp" "GltfScene.h" "GltfScene.cpp")
