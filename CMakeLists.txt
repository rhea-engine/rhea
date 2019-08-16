# CMakeList.txt: CMake-Projektdatei der obersten Ebene. Führen Sie hier die globale Konfiguration aus,
# und schließen Sie Unterprojekte ein.
#
cmake_minimum_required (VERSION 3.8)

project ("Poseidon")

find_package(Vulkan)

# Static Library Link to make .exe copyable under Windows
if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS -static)
endif()

# Schließen Sie Unterprojekte ein.
add_subdirectory ("src")
