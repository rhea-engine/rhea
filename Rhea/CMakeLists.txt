cmake_minimum_required(VERSION 3.13)

project(Rhea VERSION 0.1.0 LANGUAGES C CXX)

# Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT COMMAND rhea_register_fabric)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/rhea_fabrics.cmake)
    rhea_fabrics_begin()
    set(RHEA_OWNS_FABRIC_REGISTRY TRUE)
else()
    set(RHEA_OWNS_FABRIC_REGISTRY FALSE)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/rhea_dependencies.cmake)
rhea_resolve_dependencies()

add_subdirectory(ThirdParty)
add_subdirectory(Resources)
add_subdirectory(Fabrics)

option(RHEA_BUILD_AUDIO "Build RheaAudio" ON)
option(RHEA_BUILD_CLIENT "Build RheaClient" ON)
option(RHEA_BUILD_SAMPLES "Build Rhea samples" ${PROJECT_IS_TOP_LEVEL})
option(RHEA_WITH_GRAPHICS "Enable graphics support for Rhea" ON)
option(RHEA_BUILD_PHYSICS "Enable physics support for Rhea" ON)
option(RHEA_BUILD_BULLET "Enable bullet physics support for Rhea" ON)

if(RHEA_BUILD_AUDIO AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RheaAudio/CMakeLists.txt")
    add_subdirectory(RheaAudio)
endif()

if(RHEA_BUILD_CLIENT AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RheaClient/CMakeLists.txt")
    add_subdirectory(RheaClient)
endif()

if(RHEA_BUILD_PHYSICS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RheaPhysics/CMakeLists.txt")
    add_subdirectory(RheaPhysics)
endif()

if(RHEA_BUILD_BULLET AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RheaPhysicsBullet/CMakeLists.txt")
    add_subdirectory(RheaPhysicsBullet)
    add_subdirectory(RheaPhysicsBulletDebug)
endif()



option(RHEA_WITH_GRAPHICS "Enable graphics support for Rhea" ON)

add_library(RheaBase "src/Unit.cpp" "src/UnitTree.cpp" "src/Realm.cpp" "src/Dispatcher.cpp"  "Platform/PlatformService.cpp" 
 "src/EventInput.cpp" "src/Application.cpp" "src/Component.cpp"
"Core/Material.cpp" "Core/SceneLoader.cpp" "Core/Scene.cpp" "Core/Avatar.cpp" "Core/Position.cpp"
"Core/Transform.cpp" "Core/Shader.cpp" "Core/Item.cpp" "Core/Item2D.cpp"
"Core/WorldService.cpp" "Core/Lighting.cpp" "Core/Lighting.cpp" 
"Core/Camera.cpp" "Core/Mesh.cpp" 
"Core/Light.cpp" "Core/DirectionalLight.cpp" "Core/PointLight.cpp" 
"Core/SpotLight.cpp"
"Physics/PhysicsManagerService.cpp" "include/Rhea/Graphics/SizeEvent.h" "include/Rhea/ConfigFile.h" "src/ConfigFile.cpp" 
"src/Input/InputMap.cpp" "include/Rhea/Graphics/Window.h")
add_library(Rhea::Core ALIAS RheaBase)

target_link_libraries(RheaBase PUBLIC Meworla::Math Meworla::Orion SimpleParser RheaResources Psio)

target_include_directories(RheaBase PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# helpers for consumers
set(RHEA_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE INTERNAL "")
add_library(RheaRuntime INTERFACE)
add_library(Rhea::Runtime ALIAS RheaRuntime)

target_link_libraries(RheaRuntime INTERFACE
  Rhea::Core
)

if(RHEA_BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()

if(RHEA_OWNS_FABRIC_REGISTRY)
    rhea_fabrics_end()
endif()