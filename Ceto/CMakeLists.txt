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

include("${CMAKE_CURRENT_SOURCE_DIR}/../Rhea/cmake/rhea_assets.cmake")
rhea_copy_assets(Ceto "${CMAKE_CURRENT_SOURCE_DIR}/Assets" "ceto")
rhea_copy_assets(Ceto "${RHEA_ASSETS_DIR}" "rhea")
