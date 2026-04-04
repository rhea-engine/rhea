#include <Hippios/Renderer.h>

#if HIPPIOS_OPENGL
#include "GLRenderer/GLRenderer.h"
#endif
#if HIPPIOS_D3D11
#include "D3D11Renderer/D3D11Renderer.h"
#endif

namespace hippios
{
	Renderer::~Renderer()
	{
	}

	std::unique_ptr<Renderer> Renderer::CreateRenderer(RendererBackend backend, WindowBackend window_backend)
	{
		switch (backend) {
#if HIPPIOS_OPENGL
		case RendererBackend::OpenGL:
			return std::make_unique<hippios::GLRenderer>(window_backend);
#endif // OPENGL
#if HIPPIOS_D3D11
		case RendererBackend::D3D11:
			return std::make_unique<hippios::D3D11Renderer>();
#endif // HIPPIOS_D3D11	
		default:
			printf("Render backend not supported\n");
			exit(-1);
			return nullptr;
		}
	}
}