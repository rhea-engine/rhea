#ifndef RHEA_SCENEDEFINITIONCOMPONENT
#define RHEA_SCENEDEFINITIONCOMPONENT

#include <Rhea/Component.h>
#include <Rhea/Core/SceneResource.h>

namespace rhea
{
	class SceneDefinitionComponent : public Component
	{
		RECLASS(SceneDefinitionComponent, Component, "//meworla/scenedefinitioncomponent")
	public:
		SceneDefinitionComponent(std::string name, std::string type_name) :
			Component(name, type_name)
		{}
		SceneResource::SceneDefinition scene_def;
		std::string source_resource_id;
		ResourceContext source_resource_context;
	};
}
#endif // !RHEA_SCENEDEFINITIONCOMPONENT
