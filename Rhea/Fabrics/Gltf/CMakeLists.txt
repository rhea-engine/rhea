add_library (RheaGltf
"include/Rhea/Gltf/GlbLoader.h" 
"include/Rhea/Gltf/GltfLoader.h" 
"src/GltfLoader.cpp" 
"include/Rhea/Gltf/GltfFabric.h" 
"src/GltfFabric.cpp" 
"include/Rhea/Gltf/GltfMesh.h" 
"src/GltfMesh.cpp" 
"include/Rhea/Gltf/GltfScene.h" 
"src/GltfScene.cpp")
add_library(Rhea::Gltf ALIAS RheaGltf)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaGltf PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaGltf PRIVATE Meworla::Math Rhea::TinyGltf Rhea::Core Meworla::Orion Rhea::Animation Meworla::StbImage nlohmann_json::nlohmann_json)

# register for auto registry generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")