#ifndef RHEA_IMAGE_RESOURCE
#define RHEA_IMAGE_RESOURCE

#include "Resource.h"

namespace Rhea
{
	class ImageResource : public Resource
	{
	public:
		ImageResource(std::string basename, std::string type_name) :
			Resource(basename, type_name)
		{}
		~ImageResource();
		void load(ReadyHandler handler);

		// factory function
		static Unit* createType(std::string name, std::string type);	
	};
}


#endif // !RHEA_IMAGE_RESOURCE
