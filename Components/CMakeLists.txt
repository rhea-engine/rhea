add_subdirectory(Psio)
add_subdirectory(SimpleParser)
add_subdirectory(Math)

add_library(Components INTERFACE)
target_link_libraries(Components INTERFACE Psio SimpleParser PoseidonMath)