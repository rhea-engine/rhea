#ifndef GL_RENDERTARGET_H
#define GL_RENDERTARGET_H

#include <Hippios/RenderTarget.h>
#include "GLDepthBuffer.h"
#include <memory>
#include "GLIncludes.h"
#include <Hippios/SwapChain.h>
#include <Hippios/Renderer.h>

namespace Hippios
{
	class GLRenderTarget : public RenderTarget
	{
	public:
		GLRenderTarget();
		~GLRenderTarget();
		
		bool create(Renderer* renderer, const RenderTargetDesc& desc) override;
		bool resize(int width, int height) override;

		void Release() override;
		void setActive() override;
		void clearPass() override;
		void setViewport(ViewportDesc* desc) override;

	private:
		Renderer* m_renderer = nullptr;
		SwapChain* m_swap_chain = nullptr;
		GLuint m_framebuffer = 0;
		GLuint m_color_texture = 0;
		std::unique_ptr<GLDepthBuffer> m_depth_buffer;
		int m_width = 0;
		int m_height = 0;
		bool m_is_default = true;
	};
}
#endif // !GL_RENDERTARGET_H