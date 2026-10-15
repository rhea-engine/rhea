# Define the main app
add_executable (Ceto "Ceto.h" "Ceto.cpp" "UserPrefs.h" "UserPrefs.cpp")

# add the renderer engine
target_link_libraries(Ceto Rhea::Core Rhea::Client RheaFabrics nlohmann_json::nlohmann_json)

include("${CMAKE_CURRENT_SOURCE_DIR}/../Rhea/cmake/rhea_assets.cmake")
rhea_copy_assets(Ceto "${CMAKE_CURRENT_SOURCE_DIR}/Assets" "ceto")
rhea_copy_assets(Ceto "${RHEA_ASSETS_DIR}" "rhea")
