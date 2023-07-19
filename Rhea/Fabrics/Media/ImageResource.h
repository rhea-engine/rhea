#ifndef RHEA_IMAGE_RESOURCE
#define RHEA_IMAGE_RESOURCE

#include "Resources/Resource.h"

namespace Rhea
{
	class ImageResource : public Resource
	{
	public:
		ImageResource(std::string basename, std::string type_name) :
			Resource(basename, type_name)
		{}
		~ImageResource();

		void setup(Application* app) { Unit::setup(app); }
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::Export> getReferences();

		// handle ready event
		void ready();

		// factory function
		static Unit* createType(std::string name, std::string type);	
	};
}


#endif // !RHEA_IMAGE_RESOURCE
