cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

add_library(Orion STATIC Orion.cpp Realm.cpp Realms.cpp ResourceRequest.cpp ResourceCache.cpp Resource.cpp os/Fs.cpp FileResource.cpp FileStream.cpp "Mime.cpp" "Mime.h")
if (WIN32)
	target_sources(Orion PRIVATE os/FsWin.cpp)
endif()


