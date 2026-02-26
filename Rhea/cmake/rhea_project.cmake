# Rhea/cmake/rhea_project.cmake
# Project integration helper for Rhea.
#
# Typical usage in a consumer project:
#   include(<path>/Rhea/cmake/rhea_project.cmake)
#   include(mwpkg.cmake)   # adds module subdirectories; modules call rhea_register_fabric_module()
#   rhea_project_end()
#
# For non-mwpkg setups:
#   include(<path>/Rhea/cmake/rhea_project.cmake)
#   add_subdirectory(<path-to-module-A>)
#   add_subdirectory(<path-to-module-B>)
#   rhea_project_end()

include_guard(GLOBAL)

# Load the module system
include("${CMAKE_CURRENT_LIST_DIR}/rhea_modules.cmake")

# Allow the project to set these BEFORE including this file:
#   - RHEA_ENABLED_PROFILE_JSON
#   - RHEA_ENABLED_FABRIC_IDS
#
# Friendly default: if the project has enabled.json and no profile is set, use it.
if(NOT DEFINED RHEA_ENABLED_PROFILE_JSON)
  if(EXISTS "${CMAKE_SOURCE_DIR}/enabled.json")
    set(RHEA_ENABLED_PROFILE_JSON "${CMAKE_SOURCE_DIR}/enabled.json" CACHE FILEPATH "")
    message(STATUS "Rhea: Using enabled profile: ${RHEA_ENABLED_PROFILE_JSON}")
  endif()
endif()

# Start collecting registrations
rhea_modules_begin()

# Provide an explicit "end" hook so the project controls ordering.
function(rhea_project_end)
  # Basic sanity: ensure module subdirectories were actually added.
  # (If none registered, rhea_modules_end() will generate an empty registry, but we can warn.)
  get_property(_all_jsons GLOBAL PROPERTY RHEA_FABRIC_MODULE_JSONS)
  if(NOT _all_jsons)
    message(WARNING
      "Rhea: No fabric modules registered.\n"
      "Did you forget to add module subdirectories (e.g. include(mwpkg.cmake) or add_subdirectory(...))?\n"
      "An empty registry will be generated."
    )
  endif()

  rhea_modules_end()
endfunction()
