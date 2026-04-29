# CMakeList.txt: CMake-Projekt für "Poseidon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/src/Poseidon)

# Define the main app
add_executable (TriangleTexture "TriangleTexture.cpp")

# add the renderer engine
target_link_libraries(TriangleTexture Rhea)