add_library (RheaNet 
	"src/ClientService.cpp" 
	"src/WorldClient.cpp"
	"src/NetworkComponent.cpp"
	"src/ServerService.cpp"
	"src/ClientConnection.cpp"
	"src/LobbyServer.cpp"
	"src/WorldServer.cpp" 
	"src/LobbyClient.cpp" 
	"src/NetworkFabric.cpp"
)

add_library(rhea::Network ALIAS RheaNet)

target_include_directories(RheaNet PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaNet PRIVATE Meworla::Orion Meworla::Hippios Meworla::Hermes Rhea::Core)

# Register this fabric for GeneratedFabrics.cpp generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")