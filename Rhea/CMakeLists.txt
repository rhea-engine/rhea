cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Components/)

add_library (Rhea "Application.cpp")

target_link_libraries(Rhea Poseidon Orion)