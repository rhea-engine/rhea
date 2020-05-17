#include "Texture.h"
#include "../Unit.h"
#include "../Resource.h"

#include <Poseidon/Texture.h>

namespace Rhea
{
	void GraphicsTexture::createTexture(unsigned char* buf, size_t buflen)
	{		
		m_img = m_core->createTexture((unsigned char*)buf, buflen);
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
			elem->createTexture((unsigned char*)buf, buflen);
			unit->addElement("graphics", elem);
		}
		return elem;
	}
}