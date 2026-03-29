#include <Hippios/Renderer.h>

#if HIPPIOS_OPENGL
#include "GLRenderer/GLRenderer.h"
#endif
#if HIPPIOS_D3D11
#include "D3D11Renderer/D3D11Renderer.h"
#endif

namespace Hippios
{
	Renderer::~Renderer()
	{
	}

	std::unique_ptr<Renderer> Renderer::CreateRenderer(RendererBackend backend)
	{
		switch (backend) {
#if HIPPIOS_OPENGL
		case RendererBackend::OpenGL:
			return std::make_unique<Hippios::GLRenderer>();
#endif // OPENGL
#ifdef HIPPIOS_D3D11
		case RendererBackend::D3D11:
			return std::make_unique<Hippios::D3D11Renderer>();
#endif // HIPPIOS_D3D11	
		default:
			printf("Render backend not supported\n");
			exit(-1);
			return nullptr;
		}
	}
}