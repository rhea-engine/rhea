add_library(Psio STATIC "src/Psio.cpp"  "src/Window/Window.cpp" "src/Util.cpp" "src/Log.cpp" 
"src/Input/InputEvent.cpp" 
 "src/Net/UrlRequest/UrlRequest.cpp" "src/Net/Http/HttpNetworkTransaction.cpp" "src/Net/Socket/SocketAddress.cpp" "src/Net/Socket/IPAddress.cpp"
"src/Net/IOBuffer.cpp" "src/Net/UrlRequest/UrlResponse.cpp" "src/Net/Http/HttpStream.cpp" "src/Net/Http/HttpHeaders.cpp"
"src/Net/Http/Http.cpp" "src/Net/Http/HttpResponse.cpp" "src/Net/UrlRequest/Url.cpp" "src/Net/Network.cpp" 
"src/Net/Socket/ConnectorFactory.cpp" "src/Net/Socket/TCPConnector.cpp"
"src/Image/Image.cpp" 
"src/IPC/NamedPipe.cpp" "src/IPC/Process.cpp" "src/IPC/Pipe.cpp" "src/IPC/IO.cpp" 
"src/Fs/Fs.cpp")

add_library(Meworla::Psio ALIAS Psio)
target_include_directories(Psio PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(Psio PRIVATE Meworla::StbImage)

# options
option(PSIO_ENABLE_WIN32 "Enable Win32 backend" ON)
if (WIN32 AND PSIO_ENABLE_WIN32)
    target_compile_definitions(Psio PRIVATE PSIO_HAS_WIN32=1)
endif()

if (WIN32)
	target_sources(Psio PRIVATE 
	"src/Platform/Win32/Window/Win32Window.cpp" "src/Platform/Win32/Input/InputImpl.cpp" "src/Platform/Win32/Input/KeyboardImpl.cpp" "src/Platform/Win32/Timer.cpp" 
	"src/Platform/Win32/Net/TCPSocketWin.cpp" "src/Platform/Win32/Net/SocketWin.cpp" "src/Platform/Win32/Win32Backend.cpp" "src/Platform/Win32/Net/NetworkWin32.cpp"
	"src/Platform/Win32/Fs/FsWin.cpp")
	target_link_libraries(Psio PRIVATE Winmm wsock32 ws2_32)
elseif(UNIX)
	target_sources(Psio PRIVATE 
	"src/Platform/Unix/Net/TCPSocketUnix.cpp" "src/Platform/Unix/Net/SocketUnix.cpp" "src/Platform/Unix/Net/NetworkUnix.cpp")
endif()

# SSL
# (openssl)
if(WIN32)
	set(OPENSSL_USE_STATIC_LIBS TRUE)
endif()
find_package(OpenSSL)

if (OPENSSL_FOUND)
	target_link_libraries(Psio PRIVATE ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES})
	if (WIN32)
		target_link_libraries(Psio PRIVATE crypt32)
	endif()
	target_compile_definitions(Psio PRIVATE HAS_OPENSSL)
	target_sources(Psio PRIVATE 
	"src/Net/Socket/SSLConnector.cpp" "src/Net/Socket/SSLSocket.cpp")
	INCLUDE_DIRECTORIES( ${OPENSSL_INCLUDE_DIR} ) 
endif()

# GTest
if (TARGET GTest::gtest)
file(GLOB_RECURSE test_sources
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp
)
add_executable(psio_test ${test_sources})
target_link_libraries(psio_test gtest_main gmock_main Psio)
if(WIN32)
	target_link_libraries(psio_test Winmm wsock32 Ws2_32.lib)
endif()
add_test(psio_test ${CMAKE_CURRENT_BINARY_DIR}/psio_test)
endif() 