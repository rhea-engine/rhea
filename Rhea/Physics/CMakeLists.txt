cmake_minimum_required (VERSION 3.9.2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Rhea/ ${CMAKE_SOURCE_DIR}/ThirdParty/bullet/src)

add_library (RheaPhysics "PhysicsService.cpp" "Bullet/BtPhysicsService.cpp" "Bullet/BtBody.cpp" "Body.cpp" "PhysicsManager.cpp" 
"Bullet/BtHeightmap.h" "PhysicsHeightmap.h" "Bullet/BtHeightMap.cpp" "Bullet/BtDebugDraw.h" "Bullet/BtDebugDraw.cpp" 
"PhysicsEvent.h" "Bullet/BtCharacter.h" "Bullet/BtCharacter.cpp" "Bullet/BtGhost.cpp")

target_link_libraries(RheaPhysics BulletDynamics BulletCollision LinearMath)