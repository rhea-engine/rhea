#ifndef RHEA_OBJFILELOADER_H
#define RHEA_OBJFILELOADER_H

#include <Rhea/Resources/Resource.h>
#include <string>
#include <Orion/ResourceRequest.h>
#include <Rhea/Resources/ResourceContext.h>

namespace rhea
{
	class Application;

	class ObjFileLoader
	{
	public:
		static void load(const std::string& name, const std::string& res_id, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler);
	};
}

#endif // !RHEA_OBJFILELOADER_H
