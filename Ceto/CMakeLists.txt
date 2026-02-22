# CMakeList.txt: CMake-Projekt für "Poseidon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/ThirdParty/bullet/src)

# Define the main app
add_executable (Ceto "Ceto.h" "Ceto.cpp" "UserPrefs.h" "UserPrefs.cpp")

#include(../rhea.cmake)

# add the renderer engine
target_link_libraries(Ceto Rhea::Core RheaModules Rhea::Client  nlohmann_json::nlohmann_json)
#RheaPhysicsDebug

add_custom_command(TARGET Ceto POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/Assets
                $<TARGET_FILE_DIR:Ceto>/Data/ceto)

# meworla default modules and assets
add_custom_command(TARGET Ceto POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/Modules/Assets
                $<TARGET_FILE_DIR:Ceto>/Data/meworla)

## add render engine assets
add_custom_command(TARGET Ceto POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Rhea/Assets
        $<TARGET_FILE_DIR:Ceto>/Data/rhea)