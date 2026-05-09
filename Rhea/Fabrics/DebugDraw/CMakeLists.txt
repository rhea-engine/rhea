add_library (RheaDebugDraw STATIC    
    "src/DebugDrawFabric.cpp"
 "src/LineBatchMesh.cpp")
add_library(rhea::DebugDraw ALIAS RheaDebugDraw)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaDebugDraw PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaDebugDraw PUBLIC Rhea::Core)
# register for auto registry generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")