# Detect the engine root directory
get_filename_component(RHEA_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)

# Optional: Add core engine sources if you want
# add_subdirectory(${RHEA_DIR}/Core ${CMAKE_BINARY_DIR}/Rhea/Core)

# Detect the top-level source dir if not set
if(NOT DEFINED RHEA_PROJECT_SOURCE_DIR)
    set(RHEA_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Collect all module.json files
file(GLOB_RECURSE RHEA_MODULE_JSONS    
    "${RHEA_PROJECT_SOURCE_DIR}/MwpkgModules/*/*/module.json"
)

# Ensure Python is found
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(RHEA_GENERATED_CPP "${CMAKE_BINARY_DIR}/GeneratedModules.cpp")

# Run the generator
add_custom_command(
    OUTPUT ${RHEA_GENERATED_CPP}
    COMMAND ${Python3_EXECUTABLE} ${RHEA_DIR}/scripts/generate_module_registry.py 
        --output ${RHEA_GENERATED_CPP}
        --source-dir ${RHEA_PROJECT_SOURCE_DIR}
    DEPENDS ${RHEA_MODULE_JSONS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Rhea: Generating central module registry"
)

add_library(RheaModules ${CMAKE_BINARY_DIR}/GeneratedModules.cpp)
target_include_directories(RheaModules PUBLIC ${CMAKE_SOURCE_DIR})

foreach(json_file ${RHEA_MODULE_JSONS})
    # Get the directory containing the module.json (i.e., the module root)
    get_filename_component(module_dir "${json_file}" DIRECTORY)

    # Assume headers are in `include/`
    set(include_path "${module_dir}/include")

    # If the include path exists, add it
    if(EXISTS "${include_path}")
        target_include_directories(RheaModules PUBLIC "${include_path}")
    endif()
endforeach()