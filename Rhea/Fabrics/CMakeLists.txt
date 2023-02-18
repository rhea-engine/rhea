cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Rhea/)

add_library (RheaFabrics "Character/Character.cpp" "Character/KinematicCharacter.cpp" "Terrain/Terrain.cpp" 
"Canvas/Item2D.cpp" "Text/TextWidget.h" "Text/TextWidget.cpp" "Text/Font.h" "Text/BitmapFont.h" "Text/BitmapFont.cpp"
"GUI/GUIController.h" "GUI/GUIController.cpp" "GUI/Widget.h" "GUI/Widget.cpp" "GUI/GuiService.cpp" "GUI/Label.h"
"GUI/Label.cpp" "GUI/WidgetEvent.h" "GUI/Vector2.h" 
"Media/MediaFabrics.h" "Media/MediaFabrics.cpp" "Media/ImageResource.h" "Media/ImageResource.cpp" "Media/ImageLoader.h" 
"Media/ImageLoader.cpp"
"Core/Material.h" "Core/Material.cpp" "Text/TextLoader.h" "Text/TextLoader.cpp" "Core/SceneLoader.cpp" "Core/Scene.cpp" 
"Core/Position.h" "Core/Position.cpp" "Core/Transform.h" "Core/Transform.cpp"
"3D/Item.cpp" "3D/Camera.cpp" "3D/Mesh.cpp"  "Core/Avatar.h" "Core/Avatar.cpp")