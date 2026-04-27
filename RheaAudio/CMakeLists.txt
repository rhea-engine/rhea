add_library(RheaAudio STATIC)
add_library(Rhea::Audio ALIAS RheaAudio)

target_sources(RheaAudio PRIVATE
	"src/AudioSystemFabric.cpp"
 "src/AudioResource.cpp" "include/RheaAudio/IAudioBackend.h" "src/AudioService.cpp" "src/MiniaudioBackend.cpp" "include/RheaAudio/MiniaudioBackend.h" "src/MusicResource.cpp" "src/SoundResource.cpp")

target_include_directories(RheaAudio PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(RheaAudio PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
)

target_link_libraries(RheaAudio PUBLIC
  Rhea::Core 
)

target_compile_definitions(RheaAudio PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

#Register this fabric for GeneratedFabrics.cpp generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")