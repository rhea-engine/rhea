cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (Hermes "Core.cpp" "Socket.cpp" "IPAddress.cpp" "TcpServer.cpp" "TcpStream.cpp" "Connection.cpp" "Response.cpp" "Http" "Request.cpp" "Server.cpp" "HttpResponse.cpp")

add_subdirectory(tests)

#target_link_libraries()