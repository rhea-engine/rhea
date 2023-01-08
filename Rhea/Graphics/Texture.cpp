#include "Texture.h"
#include "../Unit.h"
#include "../Resource.h"
#include <Psio/Log.h>

#include <Poseidon/Texture.h>

namespace Rhea
{
	Poseidon::Texture* GraphicsTexture::createTexture(unsigned char* buf, size_t buflen)
	{		
		m_img = m_core->createTexture((unsigned char*)buf, buflen);
		return m_img;
	}

	Element* GraphicsTexture::createElement(Poseidon::Core *core, Unit* unit)
	{
		char* buf;
		size_t buflen;

		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		GraphicsTexture* elem = new GraphicsTexture(core);
		if (static_cast<Resource*>(unit)->getRaw(&buf, &buflen)) {
			if (!elem->createTexture((unsigned char*)buf, buflen)) {
				LOG(LOG_ERROR) << "Image " << unit->getPath() << " could not be created";
				delete elem;
				return nullptr;
			}
			unit->addElement("graphics", elem);
		}
		return elem;
	}
}