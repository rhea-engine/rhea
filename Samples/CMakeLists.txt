cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Components/)

# Targets
foreach (TARGET Triangle Cube TriangleTexture)
	add_subdirectory(${TARGET})	
endforeach ()

#add_custom_target(SampleAssets)
#add_custom_command(TARGET SampleAssets POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#                ${CMAKE_CURRENT_SOURCE_DIR}/assets
#                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Samples/assets)