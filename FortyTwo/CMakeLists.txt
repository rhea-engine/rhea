if(NOT TARGET nlohmann_json::nlohmann_json)
  include(FetchContent)

  FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

if (OPENSSL_FOUND)
	INCLUDE_DIRECTORIES( ${OPENSSL_INCLUDE_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/Components)
else()
	INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/Components)
endif()

add_executable (fortytwo "FortyTwo.cpp" "World.h" "World.cpp" "FortyTwo.h" "Accounts/User.cpp" "Accounts/AccountService.h" "Accounts/AccountService.cpp" 
"Accounts/Provider.h" "Accounts/ProviderScene.h" "Accounts/ProviderScene.cpp" "Accounts/ProviderMemory.h" "Accounts/ProviderMemory.cpp"
"Authorization/Authorization.h" "Authorization/AuthorizationService.cpp" "Authorization/AuthorizationService.h" "Authorization/AuthorizationTrust.h"
"Authorization/AuthorizationTrust.cpp" "Authorization/AuthInfo.h" "Authorization/AuthorizationSession.h" "Authorization/AuthorizationSession.cpp"
"Authorization/AuthBackend.h" "Authorization/AuthBackendFile.h" "Authorization/AuthBackendFile.cpp" "AuthHandler.cpp" "AuthHandler.h" "ConnectionHandler.h" "ConnectionHandler.cpp")
target_link_libraries(fortytwo PRIVATE Meworla::Psio Meworla::Orion Rhea Meworla::Hermes nlohmann_json::nlohmann_json)

# Tests

add_executable(f42_test "World.test.cpp" "World.cpp"
"Accounts/User.cpp" "Accounts/AccountService.h" "Accounts/AccountService.cpp" "Accounts/Provider.h" "Accounts/ProviderScene.h"
"Accounts/ProviderScene.cpp" "Accounts/ProviderScene.test.cpp" "Accounts/ProviderMemory.h" "Accounts/ProviderMemory.cpp"
"Authorization/Authorization.h" "Authorization/AuthorizationService.cpp" "Authorization/AuthorizationService.h" "Authorization/AuthorizationTrust.h" 
"Authorization/AuthorizationTrust.cpp" "Authorization/AuthInfo.h" "Authorization/AuthorizationSession.h" "Authorization/AuthorizationSession.cpp"
"Authorization/AuthBackend.h" "Authorization/AuthBackendFile.h" "Authorization/AuthBackendFile.cpp" "AuthHandler.cpp" "AuthHandler.h" "ConnectionHandler.h" "ConnectionHandler.cpp")
target_link_libraries(f42_test PRIVATE Meworla::Psio Meworla::Orion Rhea Meworla::Hermes nlohmann_json::nlohmann_json gtest_main gmock_main)
if(WIN32)
	target_link_libraries(f42_test PRIVATE Winmm wsock32 Ws2_32.lib)
endif()
add_test(f42_test ${CMAKE_CURRENT_BINARY_DIR}/f42_test)