/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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
		void load(orion::ResourceRequest* req, ReadyHandler handler);

		std::string getAsString();
	};
}

#endif // !RHEA_TEXTRESOURCE_H
