#ifndef HIPPIOS_WINDOW_H
#define HIPPIOS_WINDOW_H

#include "RenderListener.h"
#include "Viewport.h"

namespace Hippios
{
	class HippiosWindow
	{
	public:
		virtual void create() = 0;
		virtual void addViewport(Viewport *viewport) = 0;
		virtual void update() = 0;
		virtual void destroy() = 0;		
	};
}

#endif