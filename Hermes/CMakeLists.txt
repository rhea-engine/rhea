cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (Hermes "Core.cpp"  "Server.cpp" "ServerStream.h" "ServerStream.cpp" "ClientStream.h" "ClientStream.cpp" "Message.cpp" "Opcode.h" "Stream.h" "Stream.cpp"
"MessageFactory.h" "MessageFactory.cpp" "Messages/MessageCreate.h" "Messages/MessageCreate.cpp" "Messages/MessageMove.h" "Messages/MessageMove.cpp" 
"Messages/MessageTurn.h" "Messages/MessageTurn.cpp" "Messages/MessageJump.h" "Messages/MessageJump.cpp" "Messages/MessagePosition.h" "Messages/MessagePosition.cpp"
"Messages/MessageEnterWorld.h" "Messages/MessageEnterWorld.cpp" "Messages/MessageAvatarList.h" "Messages/MessageAvatarList.cpp" "Messages/MessageDestroy.h" "Messages/MessageDestroy.cpp" "Messages/MessageEnterFailed.h" "Messages/MessageEnterFailed.cpp")

add_subdirectory(tests)

# GTest

file(GLOB_RECURSE test_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)
add_executable(hermes_test ${test_sources})
target_link_libraries(hermes_test gtest_main gmock_main Hermes Psio)
if(WIN32)
	target_link_libraries(hermes_test Winmm wsock32 Ws2_32.lib)
endif()
add_test(psio_test ${CMAKE_CURRENT_BINARY_DIR}/psio_test)