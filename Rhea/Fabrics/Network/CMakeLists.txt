include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (RheaFabricsNet "ClientService.h" "ClientService.cpp" "WorldClient.h" "WorldClient.cpp" 
"NetworkComponent.h" "NetworkComponent.cpp" "NetworkDelegate.h"
"ServerService.h" "ServerService.cpp" "ClientConnection.h" 
"ClientConnection.cpp"

 "IConnectionData.h" "LobbyServer.h" "LobbyServer.cpp" "WorldServer.h" "WorldServer.cpp" "LobbyClient.h" "LobbyClient.cpp")