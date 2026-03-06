#ifndef RHEA_FABRICS_MEDIA_IMAGELOADER_H
#define RHEA_FABRICS_MEDIA_IMAGELOADER_H

#include "Rhea/Resources/Resource.h"
#include "Rhea/Resources/ResourceLoader.h"

namespace Rhea
{
	class ImageLoader {
	public:
		static void load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler);
	};
}

#endif // !RHEA_FABRICS_MEDIA_IMAGELOADER_H
