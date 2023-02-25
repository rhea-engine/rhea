#ifndef RHEA_CSSLOADER_H
#define RHEA_CSSLOADER_H

#include <Rhea/Application.h>
#include <Rhea/Resource.h>
#include "ResourceLoader.h"

namespace Rhea
{
	class CssLoader
	{
	public:
		static void load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler);
	};
}
#endif // !RHEA_CSSLOADER_H
