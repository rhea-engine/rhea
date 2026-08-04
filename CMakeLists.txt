cmake_minimum_required (VERSION 3.24)

project ("Rhea")

# Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0115 NEW)

# target selection options

option(RHEA_BUILD_ENGINE   "Build Rhea core" ON)
option(RHEA_WITH_GRAPHICS "Enable graphics support for Rhea" ON)
option(RHEA_BUILD_CLIENT "Enable Poseidon renderer support for Rhea" ${RHEA_WITH_GRAPHICS})
option(RHEA_BUILD_RENDERER "Build Poseidon renderer" ${RHEA_WITH_GRAPHICS})
if(RHEA_BUILD_CLIENT AND NOT RHEA_BUILD_RENDERER)
    message(FATAL_ERROR "RHEA_BUILD_CLIENT requires RHEA_BUILD_RENDERER.")
endif()
option(RHEA_BUILD_PHYSICS "Enable physics support for Rhea" ON)
option(RHEA_BUILD_BULLET "Enable bullet physics support for Rhea" ${RHEA_BUILD_PHYSICS})
option(RHEA_BUILD_AUDIO "Enable audio support for Rhea" ON)
# Experimental features are not enabled by default
option(RHEA_BUILD_EXPERIMENTAL "Enable experimental features for Rhea" OFF)
set(RHEA_BUILD_FORTYTWO_DEFAULT OFF)
if(RHEA_BUILD_EXPERIMENTAL AND PROJECT_IS_TOP_LEVEL)
    set(RHEA_BUILD_FORTYTWO_DEFAULT ON)
endif()
option(RHEA_BUILD_FORTYTWO "Build FortyTwo server" ${RHEA_BUILD_FORTYTWO_DEFAULT})
if (RHEA_BUILD_FORTYTWO AND NOT RHEA_BUILD_ENGINE)
    message(FATAL_ERROR "RHEA_BUILD_FORTYTWO requires RHEA_BUILD_ENGINE.")
endif()

# Only add samples and apps if in top-level directory
option(RHEA_BUILD_APPS     "Build engine apps" ${PROJECT_IS_TOP_LEVEL})
if (RHEA_BUILD_APPS AND NOT RHEA_BUILD_ENGINE)
    message(FATAL_ERROR "RHEA_BUILD_APPS requires RHEA_BUILD_ENGINE.")
endif()
option(RHEA_BUILD_SAMPLES "Build Rhea samples (high-level API)" ${PROJECT_IS_TOP_LEVEL})
if(RHEA_BUILD_SAMPLES AND NOT RHEA_BUILD_CLIENT)
    message(FATAL_ERROR "RHEA_BUILD_SAMPLES requires RHEA_BUILD_CLIENT.")
endif()
option(POSEIDON_BUILD_SAMPLES "Build Poseidon samples (low-level renderer)" ${PROJECT_IS_TOP_LEVEL})
option(RHEA_BUILD_TESTS "Build Rhea tests" ${PROJECT_IS_TOP_LEVEL})

# build options
option(RHEA_FETCH_MISSING_DEPENDENCIES "Download missing third-party dependencies" ON)
option(RHEA_ENABLE_SDL "Enable SDL" ON)
option(RHEA_SDL_STATIC "Prefer a static SDL3 library" ON)
set(PSIO_ENABLE_SDL ${RHEA_ENABLE_SDL})
set(HIPPIOS_USE_SDL ${RHEA_ENABLE_SDL})
option(RHEA_ENABLE_OPENSSL "Enable OpenSSL support in Psio" ON)
set(PSIO_ENABLE_OPENSSL ${RHEA_ENABLE_OPENSSL})

include(InstallRequiredSystemLibraries)

if(RHEA_SDL_STATIC)
    # Build SDL3 as static
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
endif()

if(RHEA_BUILD_TESTS)
    include(CTest)
    enable_testing()

    find_package(GTest CONFIG QUIET)

    if(NOT TARGET GTest::gtest)
        if(RHEA_FETCH_MISSING_DEPENDENCIES)
            # Download and unpack googletest at configure time
            FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG        v1.17.0
                GIT_SHALLOW TRUE
            )
        else()
            message(FATAL_ERROR "GoogleTest was not found. Install it or enable RHEA_FETCH_MISSING_DEPENDENCIES.")
        endif()
    endif()

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
            
# json library
find_package(nlohmann_json CONFIG QUIET)

if(NOT TARGET nlohmann_json::nlohmann_json)
    if(RHEA_FETCH_MISSING_DEPENDENCIES)
        FetchContent_Declare(
            nlohmann_json
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
            URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
        )
        FetchContent_MakeAvailable(nlohmann_json)
    else()
        message(FATAL_ERROR "nlohmann_json was not found. Install it or enable RHEA_FETCH_MISSING_DEPENDENCIES.")
    endif()
endif()

set(engine_targets)
set(headless_targets)

# Build convenience interface libraries for the engine and headless engine if we are building the engine
if(RHEA_BUILD_ENGINE)
    # Target definition for rhea engine (client version)
    add_library(RheaEngine INTERFACE)
    target_compile_features(RheaEngine INTERFACE
        cxx_std_17
    )
    # Target definition for rhea engine (headless version)
    add_library(RheaHeadless INTERFACE)
    target_compile_features(RheaHeadless INTERFACE
        cxx_std_17
    )
    add_library(Rhea::Engine ALIAS RheaEngine)
    add_library(Rhea::Headless ALIAS RheaHeadless)
endif()

set(RHEA_SDK_OWNS_FABRIC_REGISTRY FALSE)

if(NOT COMMAND rhea_register_fabric)
    include("${CMAKE_CURRENT_SOURCE_DIR}/Rhea/cmake/rhea_fabrics.cmake")
    rhea_fabrics_begin()
    set(RHEA_SDK_OWNS_FABRIC_REGISTRY TRUE)
endif()

if(RHEA_BUILD_ENGINE OR RHEA_BUILD_RENDERER)
    if(NOT TARGET Meworla::Components)
        add_subdirectory(Components)
    endif()
endif()

if(RHEA_BUILD_RENDERER)
    add_subdirectory(Hippios)
    add_subdirectory(Poseidon)
endif()

if (RHEA_BUILD_ENGINE)
    add_subdirectory(Psio)
    add_subdirectory(Orion)
    add_subdirectory(Rhea)
    
    list(APPEND engine_targets Meworla::Components Meworla::Psio Meworla::Orion Rhea::Core)
    set(headless_targets ${engine_targets})
    if (RHEA_BUILD_CLIENT)
        add_subdirectory(RheaClient)

        list(APPEND engine_targets Meworla::Hippios Meworla::Poseidon Rhea::Client)
    endif()
    if (RHEA_BUILD_PHYSICS)
        add_subdirectory(RheaPhysics)
        list(APPEND engine_targets Meworla::RheaPhysics)
        list(APPEND headless_targets Meworla::RheaPhysics)
        if (RHEA_BUILD_BULLET)
            add_subdirectory(RheaPhysicsBullet)
            add_subdirectory(RheaPhysicsBulletDebug)
            list(APPEND engine_targets Meworla::RheaPhysicsBullet Meworla::RheaPhysicsBulletDebug)
            list(APPEND headless_targets Meworla::RheaPhysicsBullet)
        endif()
    endif()
    if (RHEA_BUILD_AUDIO)
        add_subdirectory(RheaAudio)
        list(APPEND engine_targets Rhea::Audio)
    endif()
    if (RHEA_BUILD_EXPERIMENTAL)
        add_subdirectory(Hermes)
        add_subdirectory(Network)
        list(APPEND engine_targets Hermes Rhea::Network)
        list(APPEND headless_targets Hermes Rhea::Network)
    endif()
endif()

if (RHEA_BUILD_FORTYTWO)
   if(NOT TARGET Hermes)
     add_subdirectory(Hermes)
   endif()
   if (NOT TARGET Rhea::Network)
     add_subdirectory(Network)
   endif()
   add_subdirectory(FortyTwo)
endif()

if(RHEA_BUILD_APPS)
    add_subdirectory(Ceto)
endif()

# if we are building the engine add the targets to the RheaEngine interface library
if(RHEA_BUILD_ENGINE)
    foreach(tgt IN LISTS engine_targets)
        target_link_libraries(RheaEngine INTERFACE ${tgt})
    endforeach()
    foreach (tgt IN LISTS headless_targets)
        target_link_libraries(RheaHeadless INTERFACE ${tgt})
    endforeach()
endif()

# Finalize: generate registry + link enabled fabric targets into RheaFabrics
if(RHEA_SDK_OWNS_FABRIC_REGISTRY)
    rhea_fabrics_end()
endif()