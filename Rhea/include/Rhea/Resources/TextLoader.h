#ifndef RHEA_TEXTLOADER_H
#define RHEA_TEXTLOADER_H

#include "Rhea/Resources/Resource.h"
#include "Rhea/Resources/ResourceLoader.h"

namespace rhea
{
	class TextLoader {
	public:
		static void load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler);
	};
}

#endif // !RHEA_TEXTLOADER_H
