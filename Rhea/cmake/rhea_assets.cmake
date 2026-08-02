function(rhea_copy_assets target source_dir subdir_name)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "rhea_copy_runtime_assets: target '${target}' does not exist")
    endif()

    if(NOT IS_DIRECTORY "${source_dir}")
        message(FATAL_ERROR "rhea_copy_assets: source directory does not exist: ${source_dir}")
    endif()

    file(
        GLOB_RECURSE asset_files
        CONFIGURE_DEPENDS
        "${source_dir}/*"
    )

    string(MAKE_C_IDENTIFIER
        "${target}_assets_${subdir_name}"
        asset_target
    )

    set(destination
        "$<TARGET_FILE_DIR:${target}>/Data/${subdir_name}"
    )

    set(stamp
        "${CMAKE_CURRENT_BINARY_DIR}/${asset_target}_$<CONFIG>.stamp"
    )

    set(copy_script
        "${CMAKE_CURRENT_BINARY_DIR}/${asset_target}_copy_$<CONFIG>.cmake"
    )

    set(copy_script_template [=[
file(MAKE_DIRECTORY "@destination@")
execute_process(
    COMMAND "@CMAKE_COMMAND@" -E copy_directory_if_different
        "@source_dir@"
        "@destination@"
    RESULT_VARIABLE copy_result
)
if(NOT copy_result EQUAL 0)
    message(FATAL_ERROR "Failed to stage assets from '@source_dir@' to '@destination@'")
endif()
]=])
    string(CONFIGURE "${copy_script_template}" copy_script_content @ONLY)
    file(GENERATE
        OUTPUT "${copy_script}"
        CONTENT "${copy_script_content}"
    )

    add_custom_command(
        OUTPUT "${stamp}"

        COMMAND "${CMAKE_COMMAND}" -P "${copy_script}"

        COMMAND "${CMAKE_COMMAND}" -E touch
            "${stamp}"

        DEPENDS ${asset_files}
        COMMENT "Staging ${subdir_name} assets"
        VERBATIM
    )

    add_custom_target(
        "${asset_target}"
        DEPENDS "${stamp}"
    )

    add_dependencies(
        "${target}"
        "${asset_target}"
    )
endfunction()

function(stage_directory_to_target target src_dir rel_dst_dir)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR "stage_directory_to_target: target '${target}' does not exist")
  endif()

  # Resolve destination under the target's runtime output directory
  set(dst_dir "$<TARGET_FILE_DIR:${target}>/Data/${rel_dst_dir}")

  # Track changes (CONFIGURE_DEPENDS: reconfigure when files are added/removed)
  file(GLOB_RECURSE asset_files CONFIGURE_DEPENDS "${src_dir}/*")

  # A stamp file makes the command "have an output" (nicer for build tools)
  set(stamp "${CMAKE_CURRENT_BINARY_DIR}/${target}__stage__${rel_dst_dir}.stamp")

  add_custom_command(
    OUTPUT "${stamp}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${dst_dir}"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${src_dir}" "${dst_dir}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${stamp}"
    DEPENDS ${asset_files}
    COMMENT "Staging assets: ${src_dir} -> ${dst_dir}"
    VERBATIM
  )

  # Expose the stamp to the caller
  set(_stage_stamp "${stamp}" PARENT_SCOPE)
endfunction()

get_filename_component(RHEA_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(RHEA_ASSETS_DIR "${RHEA_ROOT_DIR}/Assets")
