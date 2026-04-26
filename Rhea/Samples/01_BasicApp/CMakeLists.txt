# Sample 1: minimale application
add_executable(Rhea_Sample_01_BasicApp
    BasicApp.cpp 
)

target_link_libraries(Rhea_Sample_01_BasicApp PRIVATE
    Rhea::Core Rhea::Client RheaFabrics
)

# Optional: Assets kopieren
add_custom_command(TARGET Rhea_Sample_01_BasicApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:Rhea_Sample_01_BasicApp>/Data/basic_app
)

## add render engine assets
add_custom_command(TARGET Rhea_Sample_01_BasicApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Rhea/Assets
        $<TARGET_FILE_DIR:Rhea_Sample_01_BasicApp>/Data/rhea)