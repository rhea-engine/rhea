#ifndef RHEA_SCENEDOCUMENT_H
#define RHEA_SCENEDOCUMENT_H

#include <Rhea/Resources/Resource.h>

namespace rhea
{
	class SceneDocument : public Resource
	{
		RECLASS(SceneDocument, Resource, "//rhea/components/scenedocument")
	public:
		SceneDocument(std::string name, std::string type_name) :
			Resource(name, type_name)
		{}
	};
}

#endif // !RHEA_SCENEDOCUMENT_H
