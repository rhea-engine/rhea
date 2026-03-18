function(rhea_copy_assets target source_dir subdir_name)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "rhea_copy_runtime_assets: target '${target}' does not exist")
    endif()

    if(NOT DEFINED RHEA_ASSETS_DIR)
        message(FATAL_ERROR "rhea_copy_runtime_assets: RHEA_ASSETS_DIR not set")
    endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${source_dir}"
            "$<TARGET_FILE_DIR:${target}>/Data/${subdir_name}"
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