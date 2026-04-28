cmake_minimum_required(VERSION 3.20)

add_library(vorbis STATIC
    lib/analysis.c
    lib/bitrate.c
    lib/block.c
    lib/codebook.c
    lib/envelope.c
    lib/floor0.c
    lib/floor1.c
    lib/info.c
    lib/lookup.c
    lib/lpc.c
    lib/lsp.c
    lib/mapping0.c
    lib/mdct.c
    lib/psy.c
    lib/registry.c
    lib/res0.c
    lib/sharedbook.c
    lib/smallft.c
    lib/synthesis.c
    lib/vorbisenc.c
    lib/window.c
)

target_include_directories(vorbis PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vorbis PUBLIC ogg)

set_target_properties(vorbis PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

add_library(vorbisfile STATIC
    lib/vorbisfile.c
)

target_include_directories(vorbisfile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vorbisfile PUBLIC
    vorbis
    ogg
)

set_target_properties(vorbisfile PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)