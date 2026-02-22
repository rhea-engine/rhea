add_library(RheaPhysics
  "src/NullPhysicsService.cpp"
  "src/NullBody.cpp"
  "src/NullPhysicsCharacter.cpp"  
  "Fabrics/src/RheaPhysicsFabric.cpp")

add_library(Meworla::RheaPhysics ALIAS RheaPhysics)

target_include_directories(RheaPhysics PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(RheaPhysics PUBLIC
  Rhea::Core
  Meworla::Components
)

# Register this fabric for GeneratedModules.cpp generation
rhea_register_fabric_module("${CMAKE_CURRENT_SOURCE_DIR}/Fabrics/module.json")
