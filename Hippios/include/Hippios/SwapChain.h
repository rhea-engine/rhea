#ifndef HIPPIOS_SWAPCHAIN_H
#define HIPPIOS_SWAPCHAIN_H

namespace Hippios
{
	class SwapChain
	{
	public:
		virtual ~SwapChain() {}

		virtual bool isValid() const = 0;

		// handle resize event
		virtual void onResize(int width, int height) = 0;

		// set fullscreen mode
		virtual void SetFullscreen(bool mode) = 0;

		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }

	protected:
		// cache dimensions for easy access
		int m_width = 0;
		int m_height = 0;
	};
}

#endif // !HIPPIOS_SWAPCHAIN_H
