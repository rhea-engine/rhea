# CMakeList.txt: CMake-Projekt für "Poseidon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/Poseidon)

# Define the main app
add_executable (Cube "Cube.cpp")

# add the renderer engine
target_link_libraries(Cube Rhea)

## add render engine assets
add_custom_command(TARGET Cube POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Rhea/Assets
        $<TARGET_FILE_DIR:Cube>/rhea)