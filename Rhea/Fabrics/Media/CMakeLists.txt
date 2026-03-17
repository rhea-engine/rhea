add_library (RheaMedia
    "src/ImageLoader.cpp"
    "src/MediaFabric.cpp"
)
add_library(Rhea::Media ALIAS RheaMedia)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaMedia PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaMedia PUBLIC Rhea::Core Meworla::StbImage)

# register for auto registry generation
rhea_register_fabric_module("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")