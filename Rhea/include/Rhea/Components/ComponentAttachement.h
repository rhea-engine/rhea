#ifndef RHEA_COMPONENTATTACHMENT_H
#define RHEA_COMPONENTATTACHMENT_H

#include <Rhea/Component.h>

namespace rhea
{
	class ComponentAttachment : public Component
	{
		RECLASS(ComponentAttachment, Component, "//meworla/componentattachment")
	public:
		ComponentAttachment(std::string name, std::string type_name) :
			Component(name, type_name)
		{}
	};
}

#endif // !RHEA_COMPONENTATTACHMENT_H
