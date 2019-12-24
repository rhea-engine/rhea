# CMakeList.txt: CMake-Projekt für "Poseidon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/)

# Define the main app
add_executable (Triangle "Triangle.cpp")

# add the renderer engine
target_link_libraries(Triangle Rhea)

add_custom_command(TARGET Triangle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../assets
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Samples/assets)