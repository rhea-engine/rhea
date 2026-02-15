cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Rhea/)

add_library (RheaFabrics
"Canvas/Item2D.cpp" "Text/TextWidget.h" "Text/TextWidget.cpp" "Text/Font.h" "Text/BitmapFont.h" "Text/BitmapFont.cpp"
"Media/MediaFabrics.h" "Media/MediaFabrics.cpp" "Media/ImageResource.h" "Media/ImageResource.cpp" "Media/ImageLoader.h" 
"Media/ImageLoader.cpp" "Media/CubeMap.h" "Media/CubeMap.cpp" 
"Text/TextLoader.h" "Text/TextLoader.cpp" 
"3D/Camera.cpp" "3D/Mesh.cpp" "3D/ObjLoader.h" "3D/ObjLoader.cpp" "3D/Skybox.h" "3D/Skybox.cpp" 
"3D/Light.h" "3D/Light.cpp" "3D/DirectionalLight.h" "3D/DirectionalLight.cpp" "3D/PointLight.h" "3D/PointLight.cpp" 
"3D/SpotLight.h" "3D/SpotLight.cpp")

add_subdirectory (Network)
#add_subdirectory(Animation)
target_link_libraries(RheaFabrics RheaFabricsNet Animation)

# add internal fabrics
add_subdirectory(Animation EXCLUDE_FROM_ALL)
add_subdirectory(Character EXCLUDE_FROM_ALL)
add_subdirectory(Controller EXCLUDE_FROM_ALL)
add_subdirectory(Gltf EXCLUDE_FROM_ALL)
add_subdirectory(GUI EXCLUDE_FROM_ALL)
add_subdirectory(Terrain EXCLUDE_FROM_ALL)