add_library (RheaSkybox
"src/SkyboxFabric.cpp" 
"src/Skybox.cpp"
)
add_library(Rhea::Skybox ALIAS RheaSkybox)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaSkybox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaSkybox PUBLIC Rhea::Core)

# register for auto registry generation
rhea_register_fabric_module("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")