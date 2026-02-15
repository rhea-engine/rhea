# Detect the engine root directory
get_filename_component(RHEA_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)

# Detect the top-level source dir if not set
if(NOT DEFINED RHEA_PROJECT_SOURCE_DIR)
    set(RHEA_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Collect all module.json files

# If building inside the engine repo, RHEA_DIR is inside that repo.
# If building in a consumer project, RHEA_DIR is inside the installed engine/package.
set(_rhea_repo_root "${RHEA_DIR}") 
get_filename_component(_rhea_repo_root "${_rhea_repo_root}" ABSOLUTE)

set(RHEA_GENERATOR_PY "${RHEA_DIR}/scripts/generate_module_registry.py")
set(RHEA_GENERATED_CPP "${CMAKE_BINARY_DIR}/GeneratedModules.cpp")

# Ensure Python is found
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Module roots to scan (internal + external)
set(RHEA_MODULE_ROOTS
    "${RHEA_PROJECT_SOURCE_DIR}/MwpkgModules"
    "${_rhea_repo_root}/Rhea/Fabrics"
)

# Collect module.json from all roots that exist
set(RHEA_MODULE_JSONS "")
foreach(root ${RHEA_MODULE_ROOTS})
    if(EXISTS "${root}")
        file(GLOB_RECURSE _jsons "${root}/**/module.json")
        list(APPEND RHEA_MODULE_JSONS ${_jsons})
    endif()
endforeach()
list(REMOVE_DUPLICATES RHEA_MODULE_JSONS)

set(RHEA_GENERATED_CPP "${CMAKE_BINARY_DIR}/GeneratedModules.cpp")

# Build generator args: --module-root <root> for each existing root
set(_gen_root_args "")
foreach(root ${RHEA_MODULE_ROOTS})
    if(EXISTS "${root}")
        list(APPEND _gen_root_args --module-root "${root}")
    endif()
endforeach()

# Run the generator
add_custom_command(
    OUTPUT ${RHEA_GENERATED_CPP}
    COMMAND ${Python3_EXECUTABLE} ${RHEA_GENERATOR_PY} 
        --output ${RHEA_GENERATED_CPP}
        --source-dir ${RHEA_PROJECT_SOURCE_DIR}
        ${_gen_root_args}
    DEPENDS ${RHEA_MODULE_JSONS} ${RHEA_GENERATOR_PY}
    WORKING_DIRECTORY ${RHEA_PROJECT_SOURCE_DIR}
    COMMENT "Rhea: Generating central module registry"
)

add_library(RheaModules STATIC)
target_sources(RheaModules PRIVATE ${RHEA_GENERATED_CPP})
add_custom_target(RheaModules_Gen DEPENDS ${RHEA_GENERATED_CPP})
add_dependencies(RheaModules RheaModules_Gen)

target_include_directories(RheaModules PUBLIC ${RHEA_PROJECT_SOURCE_DIR})
foreach(json_file ${RHEA_MODULE_JSONS})
    # Get the directory containing the module.json (i.e., the module root)
    get_filename_component(module_dir "${json_file}" DIRECTORY)

    # Assume headers are in `include/`
    set(include_path "${module_dir}/include")

    # If the include path exists, add it
    if(EXISTS "${include_path}")
        target_include_directories(RheaModules PUBLIC "${include_path}")
    endif()

    file(READ "${json_file}" _json_text)
    string(REGEX MATCH "\"cmake_target\"[ \t\r\n]*:[ \t\r\n]*\"([^\"]+)\"" _m "${_json_text}")
    set(_tgt "${CMAKE_MATCH_1}")

    if(_tgt AND TARGET ${_tgt})
        target_link_libraries(RheaModules PRIVATE ${_tgt})
    endif()

endforeach()