include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (Animation
	"include/Rhea/Animation/AnimationFabrics.h" 
	"src/AnimationFabrics.cpp" 
	"include/Rhea/Animation/Animation.h" 
	"src/Animation.cpp" 
	"include/Rhea/Animation/Animator.h" 
	"src/Animator.cpp" 
	"include/Rhea/Animation/Bone.h" 
	"include/Rhea/Animation/Skeleton.h" 
	"src/Skeleton.cpp"
)
target_include_directories(Animation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Animation PUBLIC RheaEngine)