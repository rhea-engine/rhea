include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR})

add_library (RheaFabricsAnimation
 "AnimationFabrics.h" "AnimationFabrics.cpp" "Animation.h" "Animation.cpp" "Animator.h" "Animator.cpp" "Bone.h" "Skeleton.h" "Skeleton.cpp"
)
