#ifndef HIPPIOS_GLSWAPCHAINSDL_H
#define HIPPIOS_GLSWAPCHAINSDL_H

#include <Hippios/SwapChain.h>
#include <SDL3/SDL.h>

namespace Hippios
{
	struct NativeWindow;

	class GLSwapChainSDL : public SwapChain
	{
	public:
		explicit GLSwapChainSDL(NativeWindow* win);
		~GLSwapChainSDL() override = default;

		bool isValid() const;

		void onResize(int width, int height) override;
		void SetFullscreen(bool mode) override;

		SDL_Window* getWindow() const { return m_window; }

	private:
		SDL_Window* m_window = nullptr;
	};
}

#endif