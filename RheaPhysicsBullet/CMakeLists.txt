# CMakeLists.txt for RheaPhysicsBullet

# add bullet physics engine as a subdirectory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/bullet/src)
add_subdirectory(ThirdParty/bullet)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/bullet/src
)

target_link_libraries(RheaPhysicsBullet PUBLIC Meworla::RheaPhysics Rhea::Core Rhea::Terrain Meworla::Orion BulletDynamics BulletSoftBody BulletCollision LinearMath)

rhea_register_fabric_module("${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/module.json")