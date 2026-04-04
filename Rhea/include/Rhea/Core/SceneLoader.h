#ifndef RHEA_SCENELOADER_H
#define RHEA_SCENELOADER_H

#include <Rhea/Resources/Resource.h>
#include <Rhea/Resources/ResourceLoader.h>

namespace rhea
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
