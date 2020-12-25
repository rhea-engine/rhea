#ifndef RHEA_SCENELOADER_H
#define RHEA_SCENELOADER_H

namespace Orion
{
	class Resource;
}

namespace Rhea
{
	class Application;
	class Unit;
	class Scene;

	class SceneLoader
	{
	public:
		void load(Scene* scene, Orion::Resource* resource, Application* app);
	};
}
#endif // !RHEA_SCENELOADER_H
