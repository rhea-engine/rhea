add_library(RheaAudio STATIC)
add_library(Rhea::Audio ALIAS RheaAudio)

target_sources(RheaAudio PRIVATE
	"src/AudioSystemFabric.cpp"
 "src/AudioResource.cpp" "include/RheaAudio/IAudioBackend.h" "src/AudioService.cpp"  "src/MusicResource.cpp" "src/SoundResource.cpp"
"src/MiniaudioBackend.cpp" "include/RheaAudio/MiniaudioBackend.h")

target_include_directories(RheaAudio PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(RheaAudio PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libvorbis
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libogg
)

option(RHEA_AUDIO_ENABLE_VORBIS "Enable OGG/Vorbis support" ON)
option(RHEA_AUDIO_USE_BUNDLED_VORBIS "Use bundled libogg/libvorbis" ON)

if(RHEA_AUDIO_ENABLE_VORBIS)
    if(RHEA_AUDIO_USE_BUNDLED_VORBIS)
        add_subdirectory(thirdparty/libogg EXCLUDE_FROM_ALL)
        add_subdirectory(thirdparty/libvorbis EXCLUDE_FROM_ALL)

        target_link_libraries(RheaAudio PRIVATE
            ogg
            vorbis
            vorbisfile
        )
    else()
        find_package(Vorbis CONFIG REQUIRED)

        target_link_libraries(RheaAudio PRIVATE
            Vorbis::vorbis
            Vorbis::vorbisfile
        )
    endif()

    target_sources(RheaAudio PRIVATE
        thirdparty/miniaudio/extras/decoders/libvorbis/miniaudio_libvorbis.c
    )

    target_compile_definitions(RheaAudio PRIVATE
        RHEA_AUDIO_HAS_VORBIS=1
    )
endif()

target_link_libraries(RheaAudio PRIVATE
  Rhea::Core 
)

target_compile_definitions(RheaAudio PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

#Register this fabric for GeneratedFabrics.cpp generation
rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/fabric.json")