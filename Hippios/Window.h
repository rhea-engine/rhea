#ifndef HIPPIOS_WINDOW_H
#define HIPPIOS_WINDOW_H

#include "RenderListener.h"
#include "Viewport.h"

namespace Psio
{
	class Engine;
}

namespace Hippios
{
	class HippiosWindow
	{
	public:
		virtual void create(Psio::Engine* psio) = 0;
		virtual void addViewport(Viewport *viewport) = 0;
		virtual void update() = 0;
		virtual void destroy() = 0;		

		virtual int getHeight() = 0;
		virtual int getWidth() = 0;
	};
}

#endif