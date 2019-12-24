#ifndef POSEIDON_TEXTURE_H
#define POSEIDON_TEXTURE_H

#include "Hippios/Engine.h"
#include "Hippios/Renderer.h"
#include "Hippios/Texture.h"

#include "Image.h"

namespace Poseidon
{
	class Texture : public Image
	{
	protected:
		Hippios::Renderer* m_renderer = 0;
		Hippios::Texture* m_hw_texture = 0;
	public:
		void bind(Hippios::Engine *engine);
		bool load(const std::string& file_name);
		Hippios::Texture* get_texture() { return m_hw_texture; };
	};
}
#endif // !POSEIDON_TEXTURE_H
