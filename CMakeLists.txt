cmake_minimum_required (VERSION 3.24)

project ("Rhea")

# Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0115 NEW)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")

message("MSVC_VERSION: ${MSVC_VERSION}")
message("MSVC_TOOLSET_VERSION: ${MSVC_TOOLSET_VERSION}")
include(InstallRequiredSystemLibraries)

option(RHEA_SDL_STATIC "Prefer a static SDL3 library" ON)
if(RHEA_SDL_STATIC)
    # Build SDL3 as static
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
endif()

# add some warnings
enable_testing()

set(WarningsLib "-Wall" "-Wpedantic" "-Wextra")
set(WarningsEXE ${WarningsLib} "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic"
    "-Wno-double-promotion" "-Wno-exit-time-destructors" "-Wno-gnu-anonymous-struct"
    "-Wno-missing-prototypes" "-Wno-nested-anon-types" "-Wno-unused-const-variable")

option(RHEA_BUILD_TESTS "Build Rhea tests" ${PROJECT_IS_TOP_LEVEL})

if(RHEA_BUILD_TESTS)
    include(CTest)
    enable_testing()

    # Download and unpack googletest at configure time
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
        GIT_SHALLOW TRUE
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
            
# json library
FetchContent_Declare(
    nlohmann_json
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
 #   URL_HASH SHA256=8c4b26bf4b422252e13f332bc5e388ec0ab5c3443d24399acb675e68278d341f
)
FetchContent_MakeAvailable(nlohmann_json)

option(RHEA_ENABLE_SDL "Enable SDL" ON)

# Target definition
add_library(RheaEngine INTERFACE)
target_compile_features(RheaEngine INTERFACE
    cxx_std_17
)

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
option(RHEA_BUILD_APPS     "Build engine apps" ${RHEA_IS_TOPLEVEL})
option(RHEA_BUILD_SAMPLES "Build Rhea samples (high-level API)" ${RHEA_IS_TOPLEVEL})
option(POSEIDON_BUILD_SAMPLES "Build Poseidon samples (low-level renderer)" ${RHEA_IS_TOPLEVEL})
option(RHEA_FETCH_MISSING_DEPENDENCIES "Download missing third-party dependencies" ON)

set(RHEA_SDK_OWNS_FABRIC_REGISTRY FALSE)

if(NOT COMMAND rhea_register_fabric)
    include("${CMAKE_CURRENT_SOURCE_DIR}/Rhea/cmake/rhea_fabrics.cmake")
    rhea_fabrics_begin()
    set(RHEA_SDK_OWNS_FABRIC_REGISTRY TRUE)
endif()

# Cross-Compiling works only for some projects
set(engine_dirs Components)
set(engine_targets Meworla::Components)
set(targetdirs FortyTwo)

list(APPEND engine_dirs Psio Hermes Hippios Orion Poseidon Rhea RheaClient RheaPhysics RheaPhysicsBullet RheaPhysicsBulletDebug Network RheaAudio)
list(APPEND engine_targets Meworla::Psio Hermes Meworla::Hippios Meworla::Orion Meworla::Poseidon Rhea::Core Rhea::Client Meworla::RheaPhysics
    Meworla::RheaPhysicsBullet Meworla::RheaPhysicsBulletDebug Rhea::Network Rhea::Audio)  


# Targets

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

if(RHEA_BUILD_APPS)    
    foreach (TARGET ${targetdirs})
	    add_subdirectory(${TARGET})	
    endforeach ()
 endif()

# Finalize: generate registry + link enabled fabric targets into RheaFabrics
if(RHEA_SDK_OWNS_FABRIC_REGISTRY)
    rhea_fabrics_end()
endif()