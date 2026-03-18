add_library(RheaPhysicsBulletDebug STATIC
  src/BtDebugDraw.cpp
  src/RheaPhysicsBulletDebugFabric.cpp
)

add_library(Meworla::RheaPhysicsBulletDebug ALIAS RheaPhysicsBulletDebug)

target_include_directories(RheaPhysicsBulletDebug PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/include
)

target_link_libraries(RheaPhysicsBulletDebug PUBLIC
  Rhea::Core
  Meworla::Poseidon
  Meworla::RheaPhysics
  Meworla::RheaPhysicsBullet
)

# Bullet libs private if not exposed in headers
target_link_libraries(RheaPhysicsBulletDebug PRIVATE
  BulletDynamics BulletCollision LinearMath
)

rhea_register_fabric("${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/module.json")