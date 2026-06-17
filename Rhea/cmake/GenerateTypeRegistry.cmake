# Type Registry - Build-Time Generation for Rhea Engine
# 
# This CMake script generates the type Rrgistry during build

cmake_minimum_required(VERSION 3.21)

# Function: Gather alll header files of a target
function(collect_target_headers target_name output_var)
	get_target_property(sources ${target_name} SOURCES)
	get_target_property(source_dir ${target_name} SOURCE_DIR)

	set(headers "")
	foreach(source ${sources})
		get_filename_component(ext ${source} EXT)
		if(ext STREQUAL ".h" OR ext STREQUAL ".hpp")
			if(IS_ABSOLUTE ${source})
				list(APPEND headers ${source})
			else()
				list(APPEND headers ${source_dir}/${source})
			endif()
		endif()
	endforeach()

	set(${output_var} ${headers} PARENT_SCOPE)
endfunction()

# Function: generate type registry for a target
function(generate_type_registry target_name)
	set(options "")
	set(oneValueArgs OUTPUT_FILE PYTHON_SCRIPT)
	set(multiValueArgs INCLUDE_DIRS ADDITIONAL_HEADERS)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	# Defaults
	if(NOT ARG_OUTPUT_FILE)
		set(ARG_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/.rhea/type_registry.json")
	endif()

	if(NOT ARG_PYTHON_SCRIPT)
		set(ARG_PYTHON_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/scan_types.py")
	endif()

	collect_target_headers(${target_name} target_headers)

	# Add additional headers
	if(ARG_ADDITIONAL_HEADERS)
		list(APPEND target_headers ${ARG_ADDITIONAL_HEADERS})
	endif()

	# cretae output folder
	get_filename_component(output_dir ${ARG_OUTPUT_FILE} DIRECTORY)
	file(MAKE_DIRECTORY ${output_dir})

	if(Python3_FOUND OR Python_FOUND)
		# Finde Python
		if(NOT Python3_EXECUTABLE AND NOT Python_EXECUTABLE)
			find_package(Python3 COMPONENTS Interpreter)
			if(NOT Python3_FOUND)
				find_package(Python COMPONENTS Interpreter)
			endif()
		endif()

		set(PYTHON_EXE ${Python3_EXECUTABLE})
		if(NOT PYTHON_EXE)
			set(PYTHON_EXE ${Python_EXECUTABLE})
		endif()

		if(PYTHON_EXE)
			# Custom target that always runs
			add_custom_target(${target_name}_generate_types ALL
				COMMAND ${PYTHON_EXE} ${ARG_PYTHON_SCRIPT} 
						${CMAKE_CURRENT_SOURCE_DIR} 
						${ARG_OUTPUT_FILE}
				DEPENDS ${target_headers} ${ARG_PYTHON_SCRIPT}
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Generating type registry for ${target_name}..."
				VERBATIM
				BYPRODUCTS ${ARG_OUTPUT_FILE}
			)

			# Automatically build before target
			add_dependencies(${target_name} ${target_name}_generate_types)

			message(STATUS "Type registry generation enabled for ${target_name}")
			message(STATUS "  Output: ${ARG_OUTPUT_FILE}")
			message(STATUS "  Using Python: ${PYTHON_EXE}")
		else()
			message(WARNING "Python not found - type registry will not be generated at build time")
		endif()
	else()
		message(STATUS "Skipping build-time type generation (Python not found)")
	endif()

	# install generated registry
	install(FILES ${ARG_OUTPUT_FILE}
			DESTINATION share/${target_name}/.rhea
			COMPONENT runtime
	)
endfunction()

# Sample usage:
# generate_type_registry(Rhea
#     OUTPUT_FILE "${CMAKE_BINARY_DIR}/Rhea/.rhea/type_registry.json"
#     PYTHON_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/scan_types.py"
# )
