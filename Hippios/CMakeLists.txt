add_library(HippiosHeaders INTERFACE)
add_library(Meworla::HippiosHeaders ALIAS HippiosHeaders)

target_include_directories(HippiosHeaders INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

add_library(Hippios STATIC)
add_library(Meworla::Hippios ALIAS Hippios)

target_sources(Hippios PRIVATE
  Renderer.cpp
 "D3D11Renderer/D3D11ShaderProgram.h")

target_link_libraries(Hippios PUBLIC
  Meworla::Components
  Meworla::HippiosHeaders
)

# Option to select the rendering backend
option(HIPPIOS_USE_OPENGL "Add OpenGL renderer" ON)
option(HIPPIOS_USE_D3D11 "Use DirectX11 renderer" ON)
option(HIPPIOS_USE_SDL "Use SDL window system" ON)
option(HIPPIOS_USE_WIN32 "Use Win32 window system" ON)

# Disable D3D11 and Win32 windows automatically on non-Windows
if(NOT WIN32)
    set(HIPPIOS_USE_D3D11 OFF CACHE BOOL "Use DirectX11 renderer" FORCE)
    set(HIPPIOS_USE_WIN32 OFF CACHE BOOL "Use Win32 window system" FORCE)
endif()

target_compile_definitions(Hippios PUBLIC
    HIPPIOS_OPENGL=$<BOOL:${HIPPIOS_USE_OPENGL}>
    HIPPIOS_D3D11=$<BOOL:${HIPPIOS_USE_D3D11}>
	HIPPIOS_SDL=$<BOOL:${HIPPIOS_USE_SDL}>,
	HIPPIOS_WIN32=$<BOOL:${HIPPIOS_USE_WIN32}>
)

if(HIPPIOS_USE_OPENGL)
  add_subdirectory(GLRenderer)
  target_link_libraries(Hippios PUBLIC GLRenderer)
endif()

if(HIPPIOS_USE_D3D11)
  # search only if right platform
  # for now only Direct3dDX11 is supported
  if(WIN32)

	if(MSVC)
		# Visual Studio 2015 is 1900, 2017 is 1910-1916, 2019 is 1920-1929, 2022 is 1930+
		if(MSVC_VERSION LESS 1900)
		  message(FATAL_ERROR "MSVC too old")
		endif()

		# Windows 10.x SDK
		get_filename_component(kit10_dir "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" REALPATH)
		file(GLOB W10SDK_VERSIONS RELATIVE ${kit10_dir}/Include ${kit10_dir}/Include/10.*) 			# enumerate pre-release and not yet known release versions
		#list(APPEND W10SDK_VERSIONS "10.0.10240.0" "10.0.14393.0" "10.0.15063.0" "10.0.16299.0")	# enumerate well known release versions
		list(REMOVE_DUPLICATES W10SDK_VERSIONS)
		list(SORT W10SDK_VERSIONS)
		list(REVERSE W10SDK_VERSIONS)																# sort from high to low
		if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
			list(INSERT W10SDK_VERSIONS 0 ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})				# prefer version passed by CMake, if any
		endif()
		foreach(W10SDK_VER ${W10SDK_VERSIONS})
			find_path(DirectX11_INCLUDE_DIR NAMES d3d11.h HINTS "${kit10_dir}/Include/${W10SDK_VER}/um" "C:/Program Files (x86)/Windows Kits/10/Include/${W10SDK_VER}/um" "C:/Program Files/Windows Kits/10/Include/${W10SDK_VER}/um")
		endforeach()
		# Windows 8.1 SDK
		if(NOT DirectX11_INCLUDE_DIR)
			find_path(DirectX11_INCLUDE_DIR NAMES d3d11.h HINTS "C:/Program Files (x86)/Windows Kits/8.1/include/um" "C:/Program Files/Windows Kits/8.1/include/um")
		endif()
		# Windows 8.0 SDK
		if(NOT DirectX11_INCLUDE_DIR)
			find_path(DirectX11_INCLUDE_DIR NAMES d3d11.h HINTS "C:/Program Files (x86)/Windows Kits/8.0/include/um" "C:/Program Files/Windows Kits/8.0/include/um")
		endif()
	endif()

	if(DirectX11_INCLUDE_DIR)
		# No need to specify full path to libraries, proper version would be found as part of SDK, in one of the following places
		# "C:/Program Files (x86)/Windows Kits/10/lib/${W10SDK_VER}/um/${MSVC_CXX_ARCHITECTURE_ID}/"
		# "C:/Program Files (x86)/Windows Kits/8.1/lib/winv6.3/um/${MSVC_CXX_ARCHITECTURE_ID}/"
		# "C:/Program Files (x86)/Windows Kits/8.0/lib/win8/um/${MSVC_CXX_ARCHITECTURE_ID}/"
		# "C:/Program Files (x86)/Windows Phone Kits/8.1/lib/${MSVC_CXX_ARCHITECTURE_ID}/"
		# "C:/Program Files (x86)/Windows Phone Kits/8.0/lib/${MSVC_CXX_ARCHITECTURE_ID}/"
		set(DirectX11_LIBRARY d3d11.lib dxgi.lib dxguid.lib d3dcompiler.lib) 
		# but it is usefull to output selected version to the log
		message(STATUS "Found DirectX11 headers: ${DirectX11_INCLUDE_DIR}")
	
		add_subdirectory(D3D11Renderer)
		target_link_libraries(Hippios PUBLIC D3D11Renderer)

	endif()
	
  endif()
endif()

if (HIPPIOS_USE_SDL)
	if(NOT TARGET SDL3::SDL3)
		find_package(SDL3 CONFIG QUIET)

	    if(NOT TARGET SDL3::SDL3-static)
	        include(FetchContent)
	        
			# Build SDL3 as static
			set(SDL_SHARED OFF CACHE BOOL "" FORCE)
			set(SDL_STATIC ON CACHE BOOL "" FORCE)

		    FetchContent_Declare(
			    SDL3
				GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
				GIT_TAG release-3.4.8
			)
			FetchContent_MakeAvailable(SDL3)
		endif()
	endif()
	# use static version of SDL3 to avoid forcing users to have SDL3 dlls in their system. Also, we don't need dynamic features of SDL3, and static linking is simpler for distribution.
	target_link_libraries(Hippios PUBLIC SDL3::SDL3-static)
endif()