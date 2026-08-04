# Define the main app
add_executable (Ceto "Ceto.h" "Ceto.cpp" "UserPrefs.h" "UserPrefs.cpp")

#include(../rhea.cmake)

# add the renderer engine
target_link_libraries(Ceto RheaEngine nlohmann_json::nlohmann_json)
#RheaPhysicsDebug

# add a headless app for testing
add_executable (Headless "Headless.cpp" "Headless.h")
target_link_libraries(Headless RheaHeadless nlohmann_json::nlohmann_json)

# option to enable dev tools
option(ENABLE_DEV_TOOLS "Enable development tools" ON)

if(ENABLE_DEV_TOOLS)
    add_definitions(-DCETO_DEV_TOOLS)
endif()

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