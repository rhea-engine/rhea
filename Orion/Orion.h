#ifndef ORION_H
#define ORION_H

#include <vector>

#include "Resource.h"
#include "Collection.h"

namespace Orion
{
	class Orion
	{
	protected:
		std::vector<Collection> m_roots;
	public:
		Orion();
		Resource* getResourceByUrl(const std::string uri);
	};
}

#endif // !ORION_H
