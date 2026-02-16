add_library(RheaPhysicsDebugPoseidon STATIC
  src/BtDebugDraw.cpp
)

add_library(Rhea::PhysicsDebugPoseidon ALIAS RheaPhysicsDebugPoseidon)

target_include_directories(RheaPhysicsDebugPoseidon PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RheaPhysicsDebugPoseidon PUBLIC
  Rhea::Client
  Rhea::PhysicsBullet
)

# Bullet libs private if not exposed in headers
target_link_libraries(RheaPhysicsDebugPoseidon PRIVATE
  BulletDynamics BulletCollision LinearMath
)

rhea_register_fabric_module("${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/DebugDraw/module.json")