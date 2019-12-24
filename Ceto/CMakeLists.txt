# CMakeList.txt: CMake-Projekt für "Poseidon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

# Define the main app
add_executable (Ceto "Ceto.cpp")

# add the renderer engine
target_link_libraries(Ceto Rhea)

add_custom_command(TARGET Ceto POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/Data
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Ceto/1.0)