#include "Texture.h"
#include "../Unit.h"
#include "../Resources/Resource.h"
#include <Psio/Log.h>
#include <Rhea/Fabrics/Media/ImageResource.h>
#include <Poseidon/Texture.h>
#include "GraphicsComponent.h"

namespace Rhea
{
	GraphicsTexture::GraphicsTexture(Poseidon::Core* core, ImageResource::TextureType texture_type, int num_textures) :
		m_type(texture_type),
		m_core(core),
		m_img(nullptr)
	{
		if (num_textures > 0) {
			if (texture_type == ImageResource::TextureType::TEXTURE_2D)
				m_img = m_core->createTexture(0, num_textures);
			else
				m_img = m_core->createTexture(1, num_textures);
		}
	}

	Poseidon::Texture* GraphicsTexture::createTexture(int index, unsigned char* buf, size_t buflen)
	{		
		m_img->load(index, (unsigned char*)buf, buflen);
		return m_img;
	}

	Poseidon::Texture* GraphicsTexture::createTexture(int index, GraphicsTexture* texture)
	{
		m_img->load(index, texture->m_img);
		return m_img;
	}


	GraphicsElement* GraphicsTexture::createElement(Poseidon::Core *core, Unit* unit)
	{
		char* buf;
		size_t buflen;

		// check if already created
		GraphicsComponent* el = unit->getComponent<GraphicsComponent>();
		if (el)
			return el->getElement();

		ImageResource* img = doCast<ImageResource>(unit);

		GraphicsTexture* elem = new GraphicsTexture(core, img->getTextureType(), img->getNumImages());

		if (img->getNumImages() == 1) {
			
			Resource* res = doCast<Resource>(img->getImage(0));
			if (!res) {
				LOG(LOG_ERROR) << "GraphicsTexture::createElement: not a resource";
				return nullptr;
			}
			if (res->getRaw(&buf, &buflen)) {
				if (!buf || !elem->createTexture(0, (unsigned char*)buf, buflen)) {
					LOG(LOG_ERROR) << "Image " << unit->getPath() << " could not be created";
					delete elem;
					return nullptr;
				}
			}
		}
		else 
		{
			for (int index = 0; index < img->getNumImages(); index++) {
				ImageResource* res = doCast<ImageResource>(img->getImage(index));
				if (!res) {
					LOG(LOG_ERROR) << "GraphicsTexture::createElement: not a resource";
					return nullptr;
				}
				GraphicsTexture* img_elem = static_cast<GraphicsTexture*>(createElement(core, res));
				if (!img_elem) {
					delete elem;
					return nullptr;
				}
				if (!elem->createTexture(index, img_elem)) {
					LOG(LOG_ERROR) << "Image " << index << " of " << unit->getPath() << " could not be added to texture";
					delete elem;
					return nullptr;
				}
			}
		}

		unit->addComponentUnique<GraphicsComponent>(elem);
		return elem;
	}
}