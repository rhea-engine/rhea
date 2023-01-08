#ifndef ORION_COLLECTION_H
#define ORION_COLLECTION_H

#include "ResourceStream.h"

namespace Orion
{
	class Entry
	{
	public:
		string name;
		string mime;
		size_t length;
	};

	class Collection : public ResourceStream
	{
	protected:
		
	public:		
		bool IsDir() { return true; }
		//virtual Resource* GetResource(string name) = 0;
		virtual Entry* next_entry() = 0;
	};
}
#endif // ! ORION_COLLECTION_H
