#ifndef RHEA_TEXTLOADER_H
#define RHEA_TEXTLOADER_H

#include "Resources/Resource.h"
#include "Resources/ResourceLoader.h"

namespace Rhea
{
	class TextLoader {
	public:
		static void load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler);
	};
}

#endif // !RHEA_TEXTLOADER_H
