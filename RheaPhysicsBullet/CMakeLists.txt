# CMakeLists.txt for RheaPhysicsBullet

set(BULLET_PHYSICS_BUILD_DEMOS OFF CACHE BOOL "Build Bullet Demos" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Build Bullet Unit Tests" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "Build Bullet2 Demos" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "Build Bullet Extras" FORCE)

include(FetchContent)
FetchContent_Declare(
    bullet3
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(bullet3)

add_library(RheaPhysicsBullet)
target_sources(RheaPhysicsBullet PRIVATE
    "src/BtBody.cpp"
    "src/BtCharacter.cpp"
    "src/BtGhost.cpp"
    "src/BtPhysicsService.cpp"
    "src/ObjTriangleLoader.cpp"
    "src/RheaPhysicsBulletFabric.cpp"  
    "src/BulletHeightMap.cpp"
    "Fabrics/include/RheaPhysicsBullet/RheaPhysicsBulletService.h")

add_library(Meworla::RheaPhysicsBullet ALIAS RheaPhysicsBullet)

target_include_directories(RheaPhysicsBullet PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/include
)

target_link_libraries(RheaPhysicsBullet PRIVATE BulletDynamics BulletSoftBody BulletCollision LinearMath)

target_include_directories(RheaPhysicsBullet PRIVATE
    "${bullet3_SOURCE_DIR}/src"
)

target_link_libraries(RheaPhysicsBullet PUBLIC Meworla::RheaPhysics Rhea::Core Rhea::Terrain Meworla::Orion Rhea::TinyObj)

rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/fabric.json")