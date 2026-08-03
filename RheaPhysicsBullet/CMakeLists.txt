# CMakeLists.txt for RheaPhysicsBullet
include(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/RheaBulletDependency.cmake"
)

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

rhea_require_bullet()

target_link_libraries(RheaPhysicsBullet PRIVATE Rhea::BulletDependencies)

target_link_libraries(RheaPhysicsBullet PUBLIC Meworla::RheaPhysics Rhea::Core Rhea::Terrain Meworla::Orion Rhea::TinyObj)

rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/fabric.json")