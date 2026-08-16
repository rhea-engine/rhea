add_library(GUI)

# Source files
target_sources(GUI PRIVATE
    src/Box.cpp
    src/Button.cpp
    src/Column.cpp
    src/CssLoader.cpp
    src/Dialog.cpp
    src/GUIFabric.cpp
    src/GUIController.cpp
    src/GuiService.cpp
    src/Label.cpp
    src/List.cpp
    src/ListItem.cpp
    src/Row.cpp
    src/TextInput.cpp
    src/Widget.cpp
    src/Styles/CssShorthand.cpp
    src/Styles/Property.cpp
    src/Styles/StyleEngine.cpp
    src/Styles/StyleSheet.cpp
    "src/ScrollView.cpp"
    "src/ScrollBar.cpp"
 "include/Rhea/GUI/Styles/DefaultStyleSheet.h" 
 "include/Rhea/GUI/Toolbar.h" "src/Toolbar.cpp")
add_library(Rhea::GUI ALIAS GUI)

# Scoped includes are under include/Rhea/GUI/**
target_include_directories(GUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(GUI PUBLIC Rhea::Core Rhea::Text)

# register for auto registry generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")