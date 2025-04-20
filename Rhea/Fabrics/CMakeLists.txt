cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Rhea/)

add_library (RheaFabrics "Character/Character.cpp" "Character/KinematicCharacter.cpp" "Terrain/Terrain.cpp" 
"Canvas/Item2D.cpp" "Text/TextWidget.h" "Text/TextWidget.cpp" "Text/Font.h" "Text/BitmapFont.h" "Text/BitmapFont.cpp"
"Media/MediaFabrics.h" "Media/MediaFabrics.cpp" "Media/ImageResource.h" "Media/ImageResource.cpp" "Media/ImageLoader.h" 
"Media/ImageLoader.cpp" "Media/CubeMap.h" "Media/CubeMap.cpp" 
"Text/TextLoader.h" "Text/TextLoader.cpp" 
"3D/Camera.cpp" "3D/Mesh.cpp" "3D/ObjLoader.h" "3D/ObjLoader.cpp" "3D/Skybox.h" "3D/Skybox.cpp" 
"GUI/Button.h" "GUI/Button.cpp" "GUI/Dialog.h" "GUI/Label.cpp" "GUI/WidgetEvent.h" 
"GUI/GUIController.h" "GUI/GUIController.cpp" "GUI/Widget.h" "GUI/Widget.cpp" "GUI/GuiService.cpp" "GUI/Label.h"
"GUI/Dialog.cpp" "GUI/List.h" "GUI/List.cpp" "GUI/ListItem.h" "GUI/ListItem.cpp" "GUI/Styles/StyleSheet.h" "GUI/Styles/StyleSheet.cpp" "GUI/CssLoader.h" "GUI/CssLoader.cpp" 
"GUI/TextInput.h" "GUI/TextInput.cpp" "GUI/Events/SubmitEvent.h" "GUI/Box.h" "GUI/Box.cpp"
"GUI/Modifier/Modifier.h"  "GUI/Modifier/Size.h" "GUI/Modifier/Offset.h" "GUI/Modifier/FillWidth.h" 
"GUI/Modifier/FillSize.h" "GUI/Modifier/Padding.h" "GUI/Column.h" "GUI/Column.cpp" "GUI/Row.h" "GUI/Row.cpp"
"GUI/Modifier/SizeWidth.h" "GUI/Modifier/SizeHeight.h" "GUI/Layout/Alignment.h" 
"3D/Light.h" "3D/Light.cpp" "3D/DirectionalLight.h" "3D/DirectionalLight.cpp" "3D/PointLight.h" "3D/PointLight.cpp" 
"3D/SpotLight.h" "3D/SpotLight.cpp" "GUI/Styles/StyleEngine.cpp" "GUI/Styles/StyleRule.h" "GUI/Styles/Property.h"
"GUI/Styles/Property.cpp" "GUI/Styles/CssShorthand.h" "GUI/Styles/CssShorthand.cpp" "GUI/Styles/CssValue.h" "GUI/Modifier/SizePercentage.h" "GUI/Modifier/SizePercentageWidth.h" 
"GUI/Modifier/SizePercentageHeight.h"  "GUI/Modifier/FillHeight.h")

add_subdirectory (Network)
add_subdirectory(Animation)
target_link_libraries(RheaFabrics RheaFabricsNet RheaFabricsAnimation)

if(USE_RHEA_GLTF_LOADER)
	add_subdirectory(Gltf)
	target_link_libraries(RheaFabrics RheaFabricsGltf)
endif()
