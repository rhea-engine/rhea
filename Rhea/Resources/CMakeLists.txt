cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Rhea/)

add_library (RheaResources "Resource.h" "Resource.cpp" "ResourceService.h" "ResourceService.cpp" "ResourceLoader.h" "ResourceLoader.cpp" "ResourceList.h" "ResourceList.cpp"
"TextResource.h" "TextResource.cpp"  "Resources.h" "Resources.cpp" "Collection.h" "Collection.cpp")