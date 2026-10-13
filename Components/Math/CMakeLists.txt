add_library(PoseidonMath "Quaternion.cpp" "Vector3.cpp" "Vector2.h" "Matrix3.cpp" "Matrix4.cpp" "Matrix3x4.cpp" "Vector4.h" "Interpolation.h" "Interpolation.cpp" "Color.h" "Color.cpp")
add_library(Meworla::Math ALIAS PoseidonMath)
target_include_directories(PoseidonMath PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

if(RHEA_BUILD_TESTS)
  add_executable(MathRotation_test tests/Vector3Rotation.test.cpp)
  target_link_libraries(MathRotation_test PRIVATE Meworla::Math)
  add_test(NAME MathRotation_test COMMAND MathRotation_test)
endif()
