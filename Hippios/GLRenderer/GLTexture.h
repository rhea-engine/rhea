#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include <Hippios/Texture.h>
#include "GLIncludes.h"

namespace Hippios
{
	class GLTexture : public Texture
	{
	public:
		GLTexture(TextureDesc& desc);
		~GLTexture();

		void loadImpl(int index, unsigned int width, unsigned int height, unsigned char* image_data) override;
		void loadCubemapFace(int faceIndex, Texture* src_texture) override;

		GLuint get() const { return m_texture; }
		GLenum getTarget() const { return m_gl_target; }

	private:
		GLuint m_texture = 0;
		GLenum m_gl_target = GL_TEXTURE_2D;
	};
}
#endif // !GL_TEXTURE_H