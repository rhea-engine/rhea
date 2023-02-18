#ifndef RHEA_SCENELOADER_H
#define RHEA_SCENELOADER_H

#include <Rhea/Resource.h>
#include "ResourceLoader.h"

namespace Rhea
{
	class Application;
	class Unit;
	class Scene;

	class SceneLoader
	{
	public:
		static void load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler);
		//void load(Scene* scene, Orion::Resource* resource, Application* app);
	};
}
#endif // !RHEA_SCENELOADER_H
