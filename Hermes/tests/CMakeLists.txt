# GTest

#set_target_properties(mwnet_http_test PROPERTIES 
# ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
# LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
# RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(mwnet_http_test HttpTest.cpp)
target_link_libraries(mwnet_http_test gtest_main gmock_main Hermes Ws2_32.lib)
add_test(mwnet_http_test ${CMAKE_CURRENT_BINARY_DIR}/mwnet_http_test)

add_executable(mwnet_connection_test ConnectionTest.cpp)
target_link_libraries(mwnet_connection_test gtest_main)
add_test(mwnet_connection_test ${CMAKE_CURRENT_BINARY_DIR}/mwnet_connection_test)