add_library(RheaAudio STATIC)
add_library(Rhea::Audio ALIAS RheaAudio)

target_sources(RheaAudio PRIVATE
	"src/AudioSystemFabric.cpp"
 "src/AudioResource.cpp")

target_include_directories(RheaAudio PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaAudio PUBLIC
  Rhea::Core 
)

#Register this fabric for GeneratedFabrics.cpp generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")