#include <Hippios/Renderer.h>
#ifdef WIN32
#include "D3D11Renderer/D3D11Renderer.h"
#endif // WIN32

namespace Hippios
{
	Renderer::~Renderer()
	{
	}

	std::unique_ptr<Renderer> Renderer::CreateRenderer()
	{
#ifdef WIN32
		return std::make_unique<Hippios::D3D11Renderer>();
#else
		printf("Only DirectX11 on Windows is supported\n");
		exit(-1);
#endif // WIN32	
		return nullptr;
	}
}