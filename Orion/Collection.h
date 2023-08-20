#ifndef ORION_COLLECTION_H
#define ORION_COLLECTION_H

#include <string>

#include "ResourceStream.h"

namespace Orion
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
		};

		bool IsDir() { return true; }

		virtual Entry* nextEntry() = 0;
	};
}
#endif // ! ORION_COLLECTION_H
