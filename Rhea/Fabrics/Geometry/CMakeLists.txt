add_library (RheaGeometry STATIC    
    "src/GeometryFabric.cpp"
    "src/CubeMesh.cpp"
    "src/PlaneMesh.cpp"
    "src/SphereMesh.cpp"
    "src/MeshBuilder.cpp"
)
add_library(rhea::Geometry ALIAS RheaGeometry)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaGeometry PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaGeometry PUBLIC Rhea::Core)
# register for auto registry generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")