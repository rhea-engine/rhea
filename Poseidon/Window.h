#ifndef POSEIDON_WINDOW_H
#define POSEIDON_WINDOW_H

#include "Viewport.h"

namespace Poseidon
{
	class Window : public Viewport
	{
	public:
		void initRenderTarget(Hippios::Engine* engine);
	};
}
#endif // !POSEIDON_WINDOW_H
