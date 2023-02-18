cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/Components)

# Define the main app
add_executable (Iris "Iris.cpp" "CommandHello.cpp" "CommandQuit.cpp" "Middleware.cpp" "CommandGet.cpp" "RegistryMemory.cpp" "Entry.cpp" "CommandError.cpp" "RegistryFileParser.cpp")

# add the renderer engine
target_link_libraries(Iris SimpleParser Hermes Ws2_32.lib Psio nlohmann_json::nlohmann_json)