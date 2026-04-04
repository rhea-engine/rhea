#ifndef RHEA_TEXTRESOURCE_H
#define RHEA_TEXTRESOURCE_H

#include "Resource.h"

namespace rhea
{
	class TextResource : public Resource
	{
		MECLASS(TextResource, Resource)
	public:
		TextResource(std::string basename, std::string type_name) :
			Resource(basename, type_name)
		{}
		~TextResource();
		void load(Orion::ResourceRequest* req, ReadyHandler handler);

		std::string getAsString();
	};
}

#endif // !RHEA_TEXTRESOURCE_H
