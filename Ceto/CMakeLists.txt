# CMakeList.txt: CMake-Projekt für "Poseidon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/ThirdParty/bullet/src)

# Define the main app
add_executable (Ceto "Ceto.cpp" "UserPrefs.h" "UserPrefs.cpp")

# add the renderer engine
target_link_libraries(Ceto RheaClient MeworlaFabrics)

add_custom_command(TARGET Ceto POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/Assets
                $<TARGET_FILE_DIR:Ceto>/Data/ceto)

# meworla default modules and assets
add_custom_command(TARGET Ceto POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/Meworla/Assets
                $<TARGET_FILE_DIR:Ceto>/Data/meworla)

## add render engine assets
add_custom_command(TARGET Ceto POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Rhea/Assets
        $<TARGET_FILE_DIR:Ceto>/Data/rhea)