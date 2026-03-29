add_subdirectory(ThirdParty/stb_image)

add_subdirectory(SimpleParser)
add_subdirectory(Math)

add_library(Components INTERFACE)
add_library(Meworla::Components ALIAS Components)
target_include_directories(Components INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Components INTERFACE Meworla::Psio Meworla::SimpleParser Meworla::Math)