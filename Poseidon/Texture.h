#ifndef POSEIDON_TEXTURE_H
#define POSEIDON_TEXTURE_H

#include "Hippios/Renderer.h"
#include "Hippios/Texture.h"

#include <Psio/Image/Image.h>

namespace Poseidon
{
	class Texture : public Image
	{
	protected:
		Hippios::Renderer* m_renderer = 0;
		Hippios::Texture* m_hw_texture = 0;
		virtual void createTexture();
	public:
		void bind(Hippios::Renderer *engine);
		
		// load an image from filesystem
		bool load(const std::string& file_name) override;

		// load an image from buffer
		virtual bool load(int index, unsigned char* buf, size_t buflen);

		// load an image from existing texture (not implemented for base class)
		virtual bool load(int index, Texture* tex);

		Hippios::Texture* get_texture() { return m_hw_texture; };
		virtual void setNumImages(int num_images) {}
	};
}
#endif // !POSEIDON_TEXTURE_H
