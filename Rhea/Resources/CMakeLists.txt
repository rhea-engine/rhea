cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Rhea/)

add_library (RheaResources "Resource.cpp" "ResourceService.cpp" "ResourceLoader.cpp" "ResourceList.cpp"
"TextResource.cpp" "Resources.cpp" "Collection.cpp" "VertexBuffer.cpp" 
 "VertexBufferStandard.cpp"  "VertexBufferSkeletal.cpp"
"ImageResource.cpp" "CubeMap.cpp"
"TextLoader.cpp")

target_link_libraries(RheaResources PRIVATE Meworla::Orion Rhea::Core)
