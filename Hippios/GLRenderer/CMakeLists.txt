# GLAD as a static library
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

add_library(GLRenderer STATIC
  GLRenderer.cpp
  GLShader.cpp
  GLVertexBuffer.cpp
  GLIndexBuffer.cpp
  GLGraphicsBuffer.cpp
  GLTexture.cpp
  GLDepthBuffer.cpp
  GLStencilState.cpp
  GLPipelineState.cpp
  GLBlendState.cpp
  GLSwapChain.cpp
  GLRenderTarget.cpp
  GLBaseTypes.cpp
  GLShaderProgram.cpp
  GLSamplerState.cpp
)

find_package(OpenGL REQUIRED)

target_include_directories(GLRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(GLRenderer
  PRIVATE
    Meworla::HippiosHeaders
    Meworla::Components
    OpenGL::GL
  PUBLIC
    glad
)