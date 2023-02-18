cmake_minimum_required(VERSION 3.18)

if (OPENSSL_FOUND)
	INCLUDE_DIRECTORIES( ${OPENSSL_INCLUDE_DIR} )
endif()

add_executable (fortytwo "FortyTwo.cpp" "World.h" "World.cpp" "ServerService.h" "ServerService.cpp" "ClientConnection.h" "ClientConnection.cpp")
target_link_libraries(fortytwo Psio Rhea Hermes)