# CMakeList.txt: CMake-Projektdatei der obersten Ebene. Führen Sie hier die globale Konfiguration aus,
# und schließen Sie Unterprojekte ein.
#
cmake_minimum_required (VERSION 3.10)

project ("Poseidon")

include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0115 OLD)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")

message("MSVC_VERSION: ${MSVC_VERSION}")
message("MSVC_TOOLSET_VERSION: ${MSVC_TOOLSET_VERSION}")
include(InstallRequiredSystemLibraries)

# add some warnings
enable_testing()

set(WarningsLib "-Wall" "-Wpedantic" "-Wextra")
set(WarningsEXE ${WarningsLib} "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic"
    "-Wno-double-promotion" "-Wno-exit-time-destructors" "-Wno-gnu-anonymous-struct"
    "-Wno-missing-prototypes" "-Wno-nested-anon-types" "-Wno-unused-const-variable")

# Download and unpack googletest at configure time
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        7e33b6a1c497ced1e98fc60175aeb4678419281c # release-1.14
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
                
# json library
FetchContent_Declare(nlohmann_json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(nlohmann_json)

# Target definition
add_library(RheaEngine INTERFACE)

# Static Library Link to make .exe copyable under Windows
if(WIN32)
#  set(CMAKE_EXE_LINKER_FLAGS -static)
endif()

# Detect if we are the top-level project
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
#if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(RHEA_IS_TOPLEVEL ON)
else()
    set(RHEA_IS_TOPLEVEL OFF)
endif()

option(RHEA_BUILD_ENGINE   "Build Rhea core" ON)
# Only add modules and apps if in top-level directory
option(RHEA_BUILD_EXTRAS  "Build bundled extras" ${RHEA_IS_TOPLEVEL})
option(RHEA_BUILD_APPS     "Build engine apps (Ceto etc.)" ${RHEA_IS_TOPLEVEL})
option(RHEA_BUILD_SAMPLES     "Build samples" ${RHEA_IS_TOPLEVEL})

# Cross-Compiling works only for some projects
set(engine_dirs Components)
set(engine_targets Meworla::Components)
set(targetdirs FortyTwo)
if(WIN32)
    list(APPEND engine_dirs Hermes Hippios Orion Poseidon Rhea RheaClient RheaPhysics RheaPhysicsBullet RheaPhysicsBulletDebug Network)
    list(APPEND engine_targets Hermes Meworla::Hippios Meworla::Orion Meworla::Poseidon Rhea::Core Rhea::Client Meworla::RheaPhysics Meworla::RheaPhysicsBullet Meworla::RheaPhysicsBulletDebug Rhea::Network)
  #  if(DEV_BUILD_CETO)
        list(APPEND targetdirs Ceto)
    #endif()
    list(APPEND targetdirs Iris)    
endif()

# Targets

# Make the functions available to subdirectories
include(Rhea/cmake/rhea_fabrics.cmake)

# Create RheaFabrics target (empty for now)
rhea_fabrics_begin()


if(RHEA_BUILD_ENGINE)
    foreach(dir IN LISTS engine_dirs)
        add_subdirectory(${dir})
    endforeach()

    foreach(tgt IN LISTS engine_targets)
        target_link_libraries(RheaEngine INTERFACE ${tgt})
    endforeach()
endif()

if(RHEA_BUILD_EXTRAS)
    add_subdirectory(Lobby)
endif()

# Finalize: generate registry + link enabled fabric targets into RheaFabrics
rhea_fabrics_end()

if(RHEA_BUILD_APPS)    
    foreach (TARGET ${targetdirs})
	    add_subdirectory(${TARGET})	
    endforeach ()
 endif()

# Setup samples
if(RHEA_BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()