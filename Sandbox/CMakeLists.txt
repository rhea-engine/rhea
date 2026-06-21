# Sample 1: minimale application
add_executable(Sandbox
    Sandbox.cpp 
)

target_link_libraries(Sandbox PRIVATE
    Rhea::Core Rhea::Client RheaFabrics rhea::Geometry Rhea::GUI
)

# Optional: Assets kopieren
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:Sandbox>/Data/sandbox
)

## add render engine assets
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../Rhea/Assets
        $<TARGET_FILE_DIR:Sandbox>/Data/rhea)