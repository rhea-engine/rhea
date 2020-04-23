include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/Components)

add_library(SimpleParser "SimpleParser.cpp" "IniFileParser.cpp")