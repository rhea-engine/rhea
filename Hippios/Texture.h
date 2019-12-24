#ifndef HIPPIOS_TEXTURE_H
#define HIPPIOS_TEXTURE_H

namespace Hippios
{
	class Renderer;

	class Texture
	{
	public:
		virtual void loadImpl(unsigned int width, unsigned int height, unsigned char* image_data) = 0;
	};
}

#endif // ! HIPPIOS_TEXTURE_H