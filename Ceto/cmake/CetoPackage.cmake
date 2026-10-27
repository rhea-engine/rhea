if(NOT PROJECT_IS_TOP_LEVEL OR NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8 OR NOT TARGET Ceto)
    message(FATAL_ERROR "Ceto packaging requires a standalone Windows x64 build with RHEA_BUILD_APPS=ON.")
endif()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "Ceto packaging requires CMAKE_BUILD_TYPE=Release.")
endif()
if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()

get_filename_component(ceto_source_dir "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

set(CETO_PACKAGE_VERSION "" CACHE STRING "Version used in the Ceto ZIP filename")
set(CETO_PACKAGE_SAMPLES_DIR "" CACHE PATH "Directory containing the sample folders, including Courtyard")
if(NOT CETO_PACKAGE_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+(-[A-Za-z0-9.-]+)?$")
    message(FATAL_ERROR "Set CETO_PACKAGE_VERSION, for example 0.1.2-rc1.")
endif()
if(NOT IS_DIRECTORY "${CETO_PACKAGE_SAMPLES_DIR}/Courtyard" OR
   NOT EXISTS "${CETO_PACKAGE_SAMPLES_DIR}/Courtyard/world.scene")
    message(FATAL_ERROR "Set CETO_PACKAGE_SAMPLES_DIR to the directory containing Courtyard/world.scene.")
endif()

# Resolve DLLs from the built executable, not from an arbitrary build-directory glob.
set(ceto_dependency_dirs "$<TARGET_FILE_DIR:Ceto>")
if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
    list(APPEND ceto_dependency_dirs "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
endif()
install(TARGETS Ceto
    RUNTIME_DEPENDENCIES
        DIRECTORIES ${ceto_dependency_dirs}
        PRE_EXCLUDE_REGEXES "[Aa][Pp][Ii]-[Mm][Ss]-.*" "[Ee][Xx][Tt]-[Mm][Ss]-.*"
        POST_EXCLUDE_REGEXES ".*[/\\\\][Ww][Ii][Nn][Dd][Oo][Ww][Ss][/\\\\].*"
    RUNTIME DESTINATION . COMPONENT CetoRuntime
    LIBRARY DESTINATION . COMPONENT CetoRuntime
)

# The root project already includes InstallRequiredSystemLibraries. Use its
# redistributable runtime list explicitly in our isolated package component.
if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        DESTINATION . COMPONENT CetoRuntime)
endif()

install(DIRECTORY "${ceto_source_dir}/Ceto/Assets/"
    DESTINATION Data/ceto COMPONENT CetoRuntime)
install(DIRECTORY "${RHEA_ASSETS_DIR}/"
    DESTINATION Data/rhea COMPONENT CetoRuntime)
install(DIRECTORY "${CETO_PACKAGE_SAMPLES_DIR}/Courtyard"
    DESTINATION Data/samples COMPONENT CetoRuntime)
install(FILES "${ceto_source_dir}/LICENSE" "${ceto_source_dir}/THIRD_PARTY_NOTICES.md"
    DESTINATION . COMPONENT CetoRuntime)
install(FILES "${ceto_source_dir}/Ceto/Packaging/README.txt"
    DESTINATION . COMPONENT CetoRuntime)

foreach(license_file IN ITEMS
    Components/ThirdParty/stb_image/LICENSE
    Rhea/ThirdParty/tiny_gltf_loader/LICENSE
    Rhea/ThirdParty/tiny_obj_loader/LICENSE
    Hippios/GLRenderer/glad/include/glad/LICENSE
    Hippios/GLRenderer/glad/include/KHR/LICENSE
    RheaAudio/thirdparty/miniaudio/LICENSE
    RheaAudio/thirdparty/stb/LICENSE
    RheaAudio/thirdparty/libogg/COPYING
    RheaAudio/thirdparty/libvorbis/COPYING)
    get_filename_component(license_dir "${license_file}" DIRECTORY)
    install(FILES "${ceto_source_dir}/${license_file}"
        DESTINATION "Licenses/${license_dir}" COMPONENT CetoRuntime)
endforeach()
if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
    install(DIRECTORY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/"
        DESTINATION Licenses/vcpkg COMPONENT CetoRuntime
        FILES_MATCHING PATTERN copyright)
endif()

set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_NAME Ceto)
set(CPACK_PACKAGE_VERSION "${CETO_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "Ceto-${CETO_PACKAGE_VERSION}-windows-x64")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
# Install only our component; omit unrelated SDK installation rules.
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};CetoRuntime;/")
include(CPack)

add_custom_target(ceto-package
    COMMAND "${CMAKE_CPACK_COMMAND}" --config "${CMAKE_BINARY_DIR}/CPackConfig.cmake" -C Release
    DEPENDS Ceto
    COMMENT "Creating the standalone Ceto ZIP package"
    VERBATIM
)

