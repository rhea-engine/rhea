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
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz DOWNLOAD_EXTRACT_TIMESTAMP)
FetchContent_MakeAvailable(json)

# Target definition
add_library(RheaEngine INTERFACE)
target_include_directories(RheaEngine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Components)

# bullet 
option(USE_PHYSICS_BULLET "Enable Bullet physics" ON)
add_subdirectory (ThirdParty/bullet)

# Static Library Link to make .exe copyable under Windows
if(WIN32)
#  set(CMAKE_EXE_LINKER_FLAGS -static)
endif()

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/Components/ ${json_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/ThirdParty/ ${PROJECT_SOURCE_DIR}/ThirdParty/bullet/src)

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
set(enginedirs Components)
set(targetdirs FortyTwo)
if(WIN32)
    list(APPEND enginedirs Poseidon Hermes Hippios Orion Rhea)    
  #  if(DEV_BUILD_CETO)
        list(APPEND targetdirs Ceto)
    #endif()
    list(APPEND targetdirs Iris)    
endif()

# Targets

if(RHEA_BUILD_ENGINE)
    foreach (ENGINE ${enginedirs})
        add_subdirectory(${ENGINE})
        target_link_libraries(RheaEngine INTERFACE ${ENGINE})
    endforeach ()

    target_include_directories(Orion PUBLIC ${json_SOURCE_DIR}/include)
 endif()

if(RHEA_BUILD_EXTRAS)
    add_subdirectory(Lobby)
endif()

if(RHEA_BUILD_APPS)    
    foreach (TARGET ${targetdirs})
	    add_subdirectory(${TARGET})	
    endforeach ()
 endif()

# Setup samples
if(RHEA_BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()