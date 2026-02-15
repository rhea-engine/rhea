# Rhea/cmake/rhea_modules.cmake
# Deterministic module registry generation
# Enabled modules are exactly those that call rhea_register_fabric_module().

include_guard(GLOBAL)

# Where to write the generated file
set(RHEA_GENERATED_CPP "${CMAKE_BINARY_DIR}/GeneratedModules.cpp" CACHE FILEPATH "")

# Accumulated module.json paths (enabled modules only)
set(RHEA_FABRIC_MODULE_JSONS "")

# Register a fabric module (call from module CMakeLists.txt)
function(rhea_register_fabric_module module_json_path)
  if(NOT IS_ABSOLUTE "${module_json_path}")
    # allow relative paths from caller
    set(module_json_path "${CMAKE_CURRENT_LIST_DIR}/${module_json_path}")
  endif()

  get_filename_component(_abs "${module_json_path}" ABSOLUTE)
  if(NOT EXISTS "${_abs}")
    message(FATAL_ERROR "rhea_register_fabric_module: module.json not found: ${_abs}")
  endif()

  # Keep a global list of registered module.json files. We use a global property to avoid issues with variable scope across subdirectories.
  get_property(_list GLOBAL PROPERTY RHEA_FABRIC_MODULE_JSONS)
  if(NOT _list)
    set(_list "")
  endif()

  list(APPEND _list "${_abs}")
  list(REMOVE_DUPLICATES _list)

  #propagate to global property so it's visible in rhea_modules_end() after all subdirs are processed
  set_property(GLOBAL PROPERTY RHEA_FABRIC_MODULE_JSONS "${_list}")  
endfunction()

# Create (or reuse) the RheaModules target and the generation rule.
# Call this once from Rhea/CMakeLists.txt *before* adding module subdirectories.
function(rhea_modules_begin)
  if(TARGET RheaModules)
    return()
  endif()

  # Set to where the generator script lives
  set(RHEA_GENERATOR_PY "${CMAKE_CURRENT_LIST_DIR}/../scripts/generate_module_registry.py")

  find_package(Python3 REQUIRED COMPONENTS Interpreter)

  # Generation rule: depends on explicit JSON list (populated later),
  # so we create the command in rhea_modules_end() when the list is final.
  add_library(RheaModules STATIC)
  
  # reset list for this configure run
  set_property(GLOBAL PROPERTY RHEA_FABRIC_MODULE_JSONS "")
endfunction()

# Finalize generation + link module targets.
# Call this once from Rhea/CMakeLists.txt *after* all module subdirectories have been added.
function(rhea_modules_end)

  if(NOT TARGET RheaModules)
    message(FATAL_ERROR "rhea_modules_end: call rhea_modules_begin() first.")
  endif()
    
  # Read the registered json list from GLOBAL property (populated by rhea_register_fabric_module calls in subdirs)
 get_property(_all_jsons GLOBAL PROPERTY RHEA_FABRIC_MODULE_JSONS)
  if(NOT _all_jsons)
    set(_all_jsons "")
    message(STATUS "RheaModules: no fabric modules registered; generating empty registry.")
  endif()

 # Build map: fabric_id -> (cmake_target, json_file)
 # fabric_id format: Vendor/Package:ModuleName  e.g. Meworla/Rhea:Gltf
 set(_known_ids "")
 set(_known_targets "")
 foreach(json_file IN LISTS _all_jsons)
   _rhea_json_get_string("${json_file}" "vendor" _v)
   _rhea_json_get_string("${json_file}" "package" _p)
   _rhea_json_get_string("${json_file}" "name" _n)
   _rhea_json_get_string("${json_file}" "cmake_target" _tgt)

   # Ignore non-fabric manifests safely (e.g. package-level module.json)
   if(NOT _tgt)
     continue()
   endif()

   if(NOT _v OR NOT _p OR NOT _n)
     message(FATAL_ERROR "Fabric module.json missing vendor/package/name: ${json_file}")
   endif()

   set(_id "${_v}/${_p}:${_n}")
   list(APPEND _known_ids "${_id}")
   list(APPEND _known_targets "${_tgt}")

   string(MAKE_C_IDENTIFIER "${_id}" _id_key)
   set_property(GLOBAL PROPERTY "RHEA_TARGET_FOR_${_id_key}" "${_tgt}")
   set_property(GLOBAL PROPERTY "RHEA_JSON_FOR_${_id_key}" "${json_file}")
 endforeach()
 list(REMOVE_DUPLICATES _known_ids)
 list(REMOVE_DUPLICATES _known_targets)

 # Decide enabled IDs (from RHEA_ENABLED_PROFILE_JSON and/or RHEA_ENABLED_FABRIC_IDS)
 _rhea_read_enabled_ids(_enabled_ids)

 # Build enabled target + json lists
 set(_enabled_targets "")
 set(_enabled_jsons "")

 if(_enabled_ids)
   foreach(id IN LISTS _enabled_ids)
     string(MAKE_C_IDENTIFIER "${id}" _id_key)
     get_property(_tgt GLOBAL PROPERTY "RHEA_TARGET_FOR_${_id_key}")
     get_property(_json GLOBAL PROPERTY "RHEA_JSON_FOR_${_id_key}")
     if(NOT _tgt OR NOT _json)
       message(FATAL_ERROR
         "Enabled fabric id '${id}' is unknown. Known ids: ${_known_ids}"
       )
     endif()
     list(APPEND _enabled_targets "${_tgt}")
     list(APPEND _enabled_jsons "${_json}")
   endforeach()
 else()
   # Default: everything registered
   # Note: use jsons that had cmake_target (fabric jsons). We'll rebuild them from known ids.
   foreach(id IN LISTS _known_ids)
     string(MAKE_C_IDENTIFIER "${id}" _id_key)
     get_property(_tgt GLOBAL PROPERTY "RHEA_TARGET_FOR_${_id_key}")
     get_property(_json GLOBAL PROPERTY "RHEA_JSON_FOR_${_id_key}")
     if(_tgt AND _json)
       list(APPEND _enabled_targets "${_tgt}")
       list(APPEND _enabled_jsons "${_json}")
     endif()
   endforeach()
 endif()

  # --- generator command uses enabled jsons only ---
  set(RHEA_GENERATOR_PY "${CMAKE_CURRENT_LIST_DIR}/../scripts/generate_module_registry.py")

  set(_json_args "")
  foreach(j IN LISTS _enabled_jsons)
    list(APPEND _json_args --module-json "${j}")
  endforeach()

  add_custom_command(
    OUTPUT "${RHEA_GENERATED_CPP}"
    COMMAND ${Python3_EXECUTABLE} "${RHEA_GENERATOR_PY}"
      --output "${RHEA_GENERATED_CPP}"
      ${_json_args}
    DEPENDS ${_enabled_jsons}
    COMMENT "Rhea: Generating central module registry (enabled fabrics)"
    VERBATIM
  )

  # Include paths for enabled modules only
  foreach(json_file IN LISTS _enabled_jsons)
    get_filename_component(module_dir "${json_file}" DIRECTORY)
    set(include_path "${module_dir}/include")
    if(EXISTS "${include_path}")
      target_include_directories(RheaModules PUBLIC "${include_path}")
    endif()
  endforeach()

  target_sources(RheaModules PRIVATE "${RHEA_GENERATED_CPP}")
  add_custom_target(RheaModules_Gen DEPENDS "${RHEA_GENERATED_CPP}")
  add_dependencies(RheaModules RheaModules_Gen)

  # Link enabled module targets only
  foreach(t IN LISTS _enabled_targets)
    if(NOT TARGET ${t})
      message(FATAL_ERROR
        "Enabled fabric target '${t}' not found. Did you add_subdirectory the module?"
      )
    endif()
    target_link_libraries(RheaModules PRIVATE ${t})
  endforeach()
endfunction()

function(_rhea_parse_cmake_target_from_json json_file out_var)
  file(READ "${json_file}" _json_text)
  string(REGEX MATCH "\"cmake_target\"[ \t\r\n]*:[ \t\r\n]*\"([^\"]+)\"" _m "${_json_text}")
  set(_tgt "${CMAKE_MATCH_1}")
  set(${out_var} "${_tgt}" PARENT_SCOPE)
endfunction()

function(_rhea_json_get_string json_file key out_var)
  file(READ "${json_file}" _txt)
  string(REGEX MATCH "\"${key}\"[ \t\r\n]*:[ \t\r\n]*\"([^\"]+)\"" _m "${_txt}")
  set(${out_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

function(_rhea_read_enabled_ids out_var)
  set(_ids "")

  if(DEFINED RHEA_ENABLED_PROFILE_JSON AND EXISTS "${RHEA_ENABLED_PROFILE_JSON}")
    file(READ "${RHEA_ENABLED_PROFILE_JSON}" _profile)
    # Very simple JSON array parser for strings:
    string(REGEX MATCHALL "\"([^\"]+)\"" _all_strings "${_profile}")

    # _all_strings includes keys too; we only want values under enabled_fabrics.
    # Simple approach: require the file to contain ONLY the array (or put array first).
    # Better approach below: match array contents specifically.
    string(REGEX MATCH "\"enabled_fabrics\"[ \t\r\n]*:[ \t\r\n]*\\[([^\\]]*)\\]" _m "${_profile}")
    set(_arr "${CMAKE_MATCH_1}")
    string(REGEX MATCHALL "\"([^\"]+)\"" _vals "${_arr}")

    foreach(s IN LISTS _vals)
      string(REGEX REPLACE "^\"|\"$" "" s "${s}")
      list(APPEND _ids "${s}")
    endforeach()
  endif()

  if(DEFINED RHEA_ENABLED_FABRIC_IDS AND RHEA_ENABLED_FABRIC_IDS)
    list(APPEND _ids ${RHEA_ENABLED_FABRIC_IDS})
  endif()

  list(REMOVE_DUPLICATES _ids)
  set(${out_var} "${_ids}" PARENT_SCOPE)
endfunction()
