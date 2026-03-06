add_library (RheaObj
    "src/ObjLoader.cpp"
    "src/ObjFabric.cpp"
)
add_library(Rhea::ObjFile ALIAS RheaObj)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaObj PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaObj PUBLIC Rhea::Core Rhea::TinyObj)

# register for auto registry generation
rhea_register_fabric_module("${CMAKE_CURRENT_SOURCE_DIR}/module.json")