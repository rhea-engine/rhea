cmake_minimum_required(VERSION 3.13)

if(TARGET Psio)
    return()
endif()

add_library(Psio STATIC "src/Psio.cpp"  "src/Window/Window.cpp" "src/Util.cpp" "src/Log.cpp" 
"src/Input/InputEvent.cpp" 
 "src/Net/UrlRequest/UrlRequest.cpp" "src/Net/Http/HttpNetworkTransaction.cpp" "src/Net/Socket/SocketAddress.cpp" "src/Net/Socket/IPAddress.cpp"
"src/Net/IOBuffer.cpp" "src/Net/UrlRequest/UrlResponse.cpp" "src/Net/Http/HttpStream.cpp" "src/Net/Http/HttpHeaders.cpp"
"src/Net/Http/Http.cpp" "src/Net/Http/HttpResponse.cpp" "src/Net/UrlRequest/Url.cpp" "src/Net/Network.cpp" 
"src/Net/Socket/ConnectorFactory.cpp" "src/Net/Socket/TCPConnector.cpp"
"src/Image/Image.cpp" 
"src/IPC/NamedPipe.cpp" "src/IPC/Process.cpp" "src/IPC/Pipe.cpp" "src/IPC/IO.cpp" 
"src/Fs/Fs.cpp" "src/Input/InputEventBuilder.cpp")

add_library(Meworla::Psio ALIAS Psio)
target_include_directories(Psio PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(Psio PRIVATE Meworla::StbImage)
target_compile_features(Psio PUBLIC cxx_std_17)

# options
option(PSIO_ENABLE_WIN32 "Enable Win32 backend" ON)
option(PSIO_ENABLE_UNIX "Enable Unix backend" ON)
option(PSIO_ENABLE_SDL "Enable SDL backend" ON)
option(PSIO_ENABLE_OPENSSL "Enable OpenSSL support" ON)

set(PSIO_NET_LOG_LEVEL "3" CACHE STRING "Network log verbosity: 0 to 4")
set_property(CACHE PSIO_NET_LOG_LEVEL PROPERTY STRINGS 0 1 2 3 4)
if(NOT PSIO_NET_LOG_LEVEL MATCHES "^[0-4]$")
    message(FATAL_ERROR "PSIO_NET_LOG_LEVEL must be an integer from 0 to 4")
endif()
target_compile_definitions(Psio PRIVATE PSIO_NET_LOG_LEVEL=${PSIO_NET_LOG_LEVEL})

if (WIN32 AND PSIO_ENABLE_WIN32)
    target_compile_definitions(Psio PRIVATE PSIO_HAS_WIN32=1)
endif()

if (UNIX AND PSIO_ENABLE_UNIX)
    target_compile_definitions(Psio PRIVATE PSIO_HAS_UNIX=1)
endif()

include(FetchContent)

if(NOT DEFINED RHEA_FETCH_MISSING_DEPENDENCIES)
    option(RHEA_FETCH_MISSING_DEPENDENCIES "Download missing third-party dependencies" ON)
endif()

if(PSIO_ENABLE_SDL)

	if(NOT TARGET SDL3::SDL3)
		find_package(SDL3 CONFIG QUIET)
	endif()

	if(NOT TARGET SDL3::SDL3)
        if (RHEA_FETCH_MISSING_DEPENDENCIES)
		    include(FetchContent)

		    FetchContent_Declare(
			    SDL3
			    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
			    GIT_TAG release-3.4.12
			    GIT_SHALLOW    TRUE
                GIT_PROGRESS   TRUE
                SYSTEM
                FIND_PACKAGE_ARGS CONFIG
		    )
		    FetchContent_MakeAvailable(SDL3)
        else()
            find_package(SDL3 CONFIG REQUIRED)
        endif()
	endif()
	
    target_link_libraries(Psio PRIVATE SDL3::SDL3)
    target_compile_definitions(Psio PRIVATE PSIO_HAS_SDL=1)

    target_sources(Psio PRIVATE
        src/Platform/SDL/SDLBackend.cpp
        src/Platform/SDL/Input/SDLInput.cpp
        src/Platform/SDL/Input/SDLKeyTranslation.cpp
        src/Platform/SDL/Window/SDLWindow.cpp
    )
endif()


if (WIN32)
	target_sources(Psio PRIVATE 
	"src/Platform/Win32/Window/Win32Window.cpp" "src/Platform/Win32/Input/Win32Input.cpp" "src/Platform/Win32/Input/Win32KeyTranslation.cpp" "src/Platform/Win32/Win32Timer.cpp" 
	"src/Platform/Win32/Net/Win32TCPSocket.cpp" "src/Platform/Win32/Net/Win32Socket.cpp" "src/Platform/Win32/Win32Backend.cpp" "src/Platform/Win32/Net/Win32Network.cpp"
	"src/Platform/Win32/Fs/Win32Fs.cpp")
	target_link_libraries(Psio PRIVATE Winmm wsock32 ws2_32)
elseif(UNIX)
	target_sources(Psio PRIVATE 
	"src/Platform/Unix/Net/UnixTCPSocket.cpp" "src/Platform/Unix/Net/UnixSocket.cpp" "src/Platform/Unix/Net/UnixNetwork.cpp"
	"src/Platform/Unix/Fs/UnixFs.cpp" "src/Platform/Unix/UnixTimer.cpp" "src/Platform/Unix/UnixBackend.cpp"
	"src/Platform/Unix/Input/UnixInput.cpp")
endif()

# SSL
# (openssl)
if(PSIO_ENABLE_OPENSSL)
    find_package(OpenSSL QUIET)
    if(OpenSSL_FOUND)
        target_link_libraries(Psio
            PRIVATE
                OpenSSL::SSL
                OpenSSL::Crypto
        )

        target_compile_definitions(Psio PRIVATE PSIO_HAS_OPENSSL=1)

        target_sources(Psio PRIVATE
            src/Net/Socket/SSLConnector.cpp
            src/Net/Socket/SSLSocket.cpp
        )
        message(STATUS "Psio: OpenSSL support enabled")
    else()
        message(STATUS "Psio: OpenSSL not found; TLS support DISABELED")
    endif()
endif()

# GTest
if (RHEA_BUILD_TESTS AND TARGET GTest::gtest)
file(GLOB_RECURSE test_sources
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp
)
if(NOT PSIO_ENABLE_OPENSSL OR NOT OpenSSL_FOUND)
    list(FILTER test_sources EXCLUDE REGEX "/SSLSocket_test\\.cpp$")
endif()
add_executable(psio_test ${test_sources})
target_link_libraries(psio_test GTest::gtest_main GTest::gmock_main Psio)
if(WIN32)
	target_link_libraries(psio_test Winmm wsock32 Ws2_32.lib)
endif()
add_test(psio_test ${CMAKE_CURRENT_BINARY_DIR}/psio_test)
endif()
