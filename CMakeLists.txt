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
FetchContent_MakeAvailable(googletest)
                
# json library
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(ThirdParty/json)

# bullet 
add_subdirectory (ThirdParty/bullet)

# Static Library Link to make .exe copyable under Windows
if(WIN32)
#  set(CMAKE_EXE_LINKER_FLAGS -static)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/Components/ ${CMAKE_SOURCE_DIR}/ThirdParty/)

# Cross-Compiling works only for some projects
set(targetdirs Components FortyTwo)
if(WIN32)
    list(APPEND targetdirs Poseidon Hippios Orion Rhea Ceto Hermes Iris Meworla)    
endif()

# Targets
foreach (TARGET ${targetdirs})
	add_subdirectory(${TARGET})	
endforeach ()

# Setup samples
add_subdirectory(Samples)