/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_COLLECTION_H
#define ORION_COLLECTION_H

#include <string>

#include <Orion/ResourceStream.h>

namespace orion
{
	class Collection : public ResourceStream
	{
	protected:
		
	public:		
		struct Entry
		{
			std::string name;
			std::string mime;
			size_t length;
			bool is_dir;
		};

		bool IsDir() { return true; }

		virtual Entry* nextEntry() = 0;
	};
}
#endif // ! ORION_COLLECTION_H
