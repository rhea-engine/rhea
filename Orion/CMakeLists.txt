cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

add_library(Orion STATIC Orion.cpp Resource.cpp BufferResource.cpp)

