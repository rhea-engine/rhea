
# Includes
#target_include_directories(HippiosD3D11Renderer PRIVATE "./")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/Poseidon ${DirectX11_INCLUDE_DIR})
#link_directories(${DirectX11_LIBRARY_DIR})

add_library(D3D11Renderer STATIC D3D11Renderer.cpp
 D3D11ShaderProgram.cpp D3D11VertexBuffer.cpp D3D11Texture.cpp D3D11GraphicsBuffer.cpp D3D11IndexBuffer.cpp D3D11DepthBuffer.cpp 
"D3D11StencilState.h" "D3D11StencilState.cpp" "D3D11PipelineState.h" "D3D11PipelineState.cpp" "D3D11BlendState.h" "D3D11BlendState.cpp" "D3D11BaseTypes.h"
"D3D11BaseType.cpp" "D3D11SwapChain.h" "D3D11SwapChain.cpp" "D3D11RenderTarget.h" "D3D11RenderTarget.cpp")

## External lib: DirectX 11

# DirectX includes should be include dirs, not link_directories
target_include_directories(D3D11Renderer PRIVATE ${DirectX11_INCLUDE_DIR})

target_link_libraries(D3D11Renderer PRIVATE ${DirectX11_LIBRARIES})

# Get Hippios public headers (no cycle because this is INTERFACE only)
target_link_libraries(D3D11Renderer PRIVATE Meworla::HippiosHeaders Meworla::Components)


