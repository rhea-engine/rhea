#include <cstdint>
#ifndef HIPPIOS_TEXTURE_DESC_H
#define HIPPIOS_TEXTURE_DESC_H

namespace Hippios
{
	struct TextureDesc
	{
		enum class TextureType
		{
			TEXTURE_1D,
			TEXTURE_2D,
			TEXTURE_2D_ARRAY,
			TEXTURE_3D,
			TEXTURE_3D_ARRAY,
			TEXTURE_CUBE,
			TEXTURE_CUBE_ARRAY,
		};

		TextureType type;
		uint32_t width;
		uint32_t height;
		uint32_t depth;
		uint32_t array_size;
	};
}
#endif // !HIPPIOS_TEXTURE_DESC_H
