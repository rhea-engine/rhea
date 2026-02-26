add_library (Hermes "src/ServerStream.cpp" "src/ClientStream.cpp" "src/Message.cpp"  "src/Stream.cpp"
"src/MessageFactory.cpp" "src/Messages/MessageCreate.cpp" "src/Messages/MessageMove.cpp" 
"src/Messages/MessageTurn.cpp" "src/Messages/MessageJump.cpp" "src/Messages/MessagePosition.cpp"
"src/Messages/MessageEnterWorld.cpp" "src/Messages/MessageAvatarList.cpp"
"src/Messages/MessageDestroy.cpp" "src/Messages/MessageEnterFailed.cpp" "src/Messages/MessageLogin.cpp" 
"src/Messages/MessageConnect.cpp" "src/Messages/MessageLoginFailed.cpp"
"src/Messages/MessageCreateUser.cpp" "src/Messages/MessageCreateAvatar.cpp" )
target_link_libraries(Hermes Meworla::Psio)
add_library(Meworla::Hermes ALIAS Hermes)

target_include_directories(Hermes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# GTest
if (TARGET GTest::gtest)
file(GLOB_RECURSE test_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.test.cpp)
add_executable(hermes_test ${test_sources} "src/Messages/MessageLogin.cpp" "src/Messages/MessageConnect.cpp" 
	"src/Messages/MessageLoginFailed.cpp" "src/Messages/MessageCreateUser.cpp" "src/Messages/MessageCreateAvatar.cpp" 
	"src/ServerStream.test.cpp" )
target_link_libraries(hermes_test gtest_main gmock_main Hermes Psio)
if(WIN32)
	target_link_libraries(hermes_test Winmm wsock32 Ws2_32.lib)
endif()
add_test(psio_test ${CMAKE_CURRENT_BINARY_DIR}/psio_test)
endif()