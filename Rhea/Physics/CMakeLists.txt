add_library(RheaPhysicsAPI STATIC 
	PhysicsManagerService.cpp
 "PhysicsBodyDescriptor.h")
add_library(Rhea::Physics ALIAS RheaPhysicsAPI)

#target_include_directories(RheaPhysicsAPI PUBLIC
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#  $<INSTALL_INTERFACE:include>
#)

target_link_libraries(RheaPhysicsAPI PRIVATE Meworla::Math Rhea::Core)