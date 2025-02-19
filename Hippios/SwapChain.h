#ifndef HIPPIOS_SWAPCHAIN_H
#define HIPPIOS_SWAPCHAIN_H

namespace Hippios
{
	class SwapChain
	{
	public:
		virtual ~SwapChain() {}

		// handle resize event
		virtual void onResize(int width, int height) = 0;
	};
}

#endif // !HIPPIOS_SWAPCHAIN_H
