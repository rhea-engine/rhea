add_library (RheaText
"src/TextFabric.cpp" 
"src/TextWidget.cpp"
"src/BitmapFont.cpp"
)
add_library(Rhea::Text ALIAS RheaText)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(RheaText PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaText PUBLIC Rhea::Core)

# register for auto registry generation
rhea_register_fabric_module("${CMAKE_CURRENT_SOURCE_DIR}/module.json")