#ifndef REA_GRAPHICS_TEXTURE_H
#define REA_GRAPHICS_TEXTURE_H

#include <cstdint>

#include <Psio/Image/Image.h>
#include <Poseidon/Core.h>

#include "../Element.h"

namespace Rhea
{
	class Unit;

	class GraphicsTexture : public Element
	{
	public:
		GraphicsTexture(Poseidon::Core *core) :
			m_core(core),
			m_img(nullptr)
		{}

		// create a texture from buffer
		Poseidon::Texture* createTexture(unsigned char* buf, size_t buflen);

		// get the Poseidon Texture
		Poseidon::Texture* get() { return m_img; }

		static Element* createElement(Poseidon::Core* core, Unit *unit);

	protected:
		Poseidon::Core* m_core;
		Poseidon::Texture* m_img;
	};
}

#endif // !REA_GRAPHICS_TEXTURE_H
