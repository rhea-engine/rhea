# CMakeList.txt: CMake-Projektdatei der obersten Ebene. Führen Sie hier die globale Konfiguration aus,
# und schließen Sie Unterprojekte ein.
#
cmake_minimum_required (VERSION 3.9.2)

project ("Poseidon")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")

# add some warnings
set(WarningsLib "-Wall" "-Wpedantic" "-Wextra")
set(WarningsEXE ${WarningsLib} "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic"
    "-Wno-double-promotion" "-Wno-exit-time-destructors" "-Wno-gnu-anonymous-struct"
    "-Wno-missing-prototypes" "-Wno-nested-anon-types" "-Wno-unused-const-variable")

# Static Library Link to make .exe copyable under Windows
if(WIN32)
#  set(CMAKE_EXE_LINKER_FLAGS -static)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/Components/)

# Targets
foreach (TARGET Poseidon Components Hippios Orion Rhea Ceto)
	add_subdirectory(${TARGET})	
endforeach ()


# Setup samples
add_subdirectory(Samples)