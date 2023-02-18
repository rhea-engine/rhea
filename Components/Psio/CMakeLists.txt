cmake_minimum_required(VERSION 3.18)

add_library(Psio STATIC Psio.cpp  Window.cpp "InputEvent.cpp"  "Net/UrlRequest/UrlRequest.cpp" 
"Net/Http/HttpTransaction.h" "Net/Http/HttpNetworkTransaction.h" "Net/Http/HttpNetworkTransaction.cpp"
"Net/Network.h" "Net/Socket/Socket.h" "Net/Socket/TCPSocket.h" "Net/Platform.h" "Net/Socket/SocketAddress.h" 
"Net/Socket/SocketAddress.cpp" "Net/Socket/IPAddress.h" "Net/Socket/IPAddress.cpp"
"Net/IOBuffer.cpp" "Stream.h" "Net/UrlRequest/UrlResponse.h" "Net/UrlRequest/UrlResponse.cpp"
"Net/Http/HttpStream.h" "Net/Http/HttpStream.cpp" "Net/Http/HttpHeaders.h" "Net/Http/HttpHeaders.cpp" "Net/IOBuffer.h" 
"Net/Http/Http.cpp" "Net/Http/HttpResponse.cpp"
"Util.cpp" "Net/UrlRequest/Url.h" "Net/UrlRequest/Url.cpp" "Net/Http/HttpRequest.h" "Net/Network.cpp" "Log.h" "Log.cpp" 
"Net/Socket/ConnectorFactory.cpp" "Net/Socket/TCPConnector.h"  "Net/Socket/TCPConnector.cpp" "Net/Socket/StreamSocket.h"
"Image/Image.h" "Image/Image.cpp")
if (WIN32)
	target_sources(Psio PRIVATE 
	"Win32/Win32Window.cpp" "Win32/InputImpl.cpp" "Win32/KeyboardImpl.cpp" "Win32/Timer.cpp" 
	"Net/Socket/TCPSocketWin.cpp" "Net/Socket/SocketWin.cpp")
	target_link_libraries(Psio PRIVATE Winmm wsock32 ws2_32)
elseif(UNIX)
	target_sources(Psio PRIVATE 
	"Net/Socket/TCPSocketUnix.cpp" "Net/Socket/SocketUnix.cpp")
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
	"Net/Socket/SSLConnector.h" "Net/Socket/SSLConnector.cpp" "Net/Socket/SSLSocket.h" "Net/Socket/SSLSocket.cpp")
	INCLUDE_DIRECTORIES( ${OPENSSL_INCLUDE_DIR} ) 
endif()

# GTest

file(GLOB_RECURSE test_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*_test.cpp)
add_executable(psio_test ${test_sources})
target_link_libraries(psio_test gtest_main gmock_main Psio)
if(WIN32)
	target_link_libraries(psio_test Winmm wsock32 Ws2_32.lib)
endif()
add_test(psio_test ${CMAKE_CURRENT_BINARY_DIR}/psio_test)