#ifndef RHEA_IMAGE_RESOURCE
#define RHEA_IMAGE_RESOURCE

#include <Rhea/Resources/Resource.h>

namespace Rhea
{
	class ImageResource : public Resource
	{
		RECLASS(ImageResource, Resource, "//meworla/image")
	public:
		ImageResource(std::string basename, std::string type_name) :
			Resource(basename, type_name)
		{}
		~ImageResource();

		enum TextureType
		{
			TEXTURE_2D = 0,
			TEXTURE_3D = 1,
			TEXTURE_CUBE = 2
		};

		void setup(Application* app) { Unit::setup(app); }
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::Export> getReferences();

		// handle ready event
		void ready();

		virtual TextureType getTextureType() { return TEXTURE_2D; }

		virtual int getNumImages() { return 1; }

		virtual Resource* getImage(int index) { if (!index) return this; else return nullptr; }
	};
}


#endif // !RHEA_IMAGE_RESOURCE
