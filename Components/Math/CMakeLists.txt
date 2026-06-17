add_library(PoseidonMath "Quaternion.cpp" "Vector3.cpp" "Vector2.h" "Matrix3.cpp" "Matrix4.cpp" "Matrix3x4.cpp" "Vector4.h" "Interpolation.h" "Interpolation.cpp" "Color.h" "Color.cpp")
add_library(Meworla::Math ALIAS PoseidonMath)
target_include_directories(PoseidonMath PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)