#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (Animation
	"include/Rhea/Animation/AnimationFabric.h" 
	"src/AnimationFabric.cpp" 
	"include/Rhea/Animation/Animation.h" 
	"src/Animation.cpp" 
	"include/Rhea/Animation/Animator.h" 
	"src/Animator.cpp" 
	"include/Rhea/Animation/Bone.h" 
	"include/Rhea/Animation/Skeleton.h" 
	"src/Skeleton.cpp"
)
add_library(Rhea::Animation ALIAS Animation)
target_include_directories(Animation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Animation PUBLIC RheaEngine)

# register for auto registry generation
rhea_register_fabric_module("${CMAKE_CURRENT_SOURCE_DIR}/module.json")