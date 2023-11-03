cmake_minimum_required(VERSION 3.18)

if (OPENSSL_FOUND)
	INCLUDE_DIRECTORIES( ${OPENSSL_INCLUDE_DIR} )
endif()

add_executable (fortytwo "FortyTwo.cpp" "World.h" "World.cpp" "ServerService.h" "ServerService.cpp" "ClientConnection.h" 
"ClientConnection.cpp" "FortyTwo.h" "Accounts/User.cpp" "Accounts/AccountService.h" "Accounts/AccountService.cpp" 
"Accounts/Provider.h" "Accounts/ProviderScene.h" "Accounts/ProviderScene.cpp" "Accounts/ProviderMemory.h" "Accounts/ProviderMemory.cpp")
target_link_libraries(fortytwo Psio Rhea Hermes)

# Tests

add_executable(f42_test "World.test.cpp" "World.cpp" "ServerService.cpp" "ClientConnection.cpp" "ClientConnection.test.cpp" 
"Accounts/User.cpp" "Accounts/AccountService.h" "Accounts/AccountService.cpp" "Accounts/Provider.h" "Accounts/ProviderScene.h"
"Accounts/ProviderScene.cpp" "Accounts/ProviderScene.test.cpp" "Accounts/ProviderMemory.h" "Accounts/ProviderMemory.cpp")
target_link_libraries(f42_test Psio Rhea Hermes gtest_main gmock_main)
if(WIN32)
	target_link_libraries(f42_test Winmm wsock32 Ws2_32.lib)
endif()
add_test(f42_test ${CMAKE_CURRENT_BINARY_DIR}/f42_test)