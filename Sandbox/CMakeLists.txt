cmake_minimum_required(VERSION 3.24)
project(Sandbox)

if(PROJECT_IS_TOP_LEVEL)
    include("${RHEA_CMAKE_MODULE_DIR}/rhea_project.cmake")
endif()

# Sample 1: minimale application
add_executable(Sandbox
    Sandbox.cpp 
)

target_link_libraries(Sandbox PRIVATE
    Rhea::Core Rhea::Client RheaFabrics rhea::Geometry Rhea::GUI
)

# Optional: Assets kopieren

rhea_copy_assets(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/Assets" "sandbox")
rhea_copy_assets(Sandbox "${RHEA_ASSETS_DIR}" "rhea")

if(PROJECT_IS_TOP_LEVEL)
    rhea_project_end()
endif()
