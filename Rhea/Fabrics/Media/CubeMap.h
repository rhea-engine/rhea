#ifndef RHEA_FABRICS_CUBEMAP_H
#define RHEA_FABRICS_CUBEMAP_H

#include "ImageResource.h"

namespace Rhea
{
	class CubeMap : public ImageResource
	{
		MECLASS(CubeMap, ImageResource)
	public:
		CubeMap(std::string name, std::string type_name) :
			m_setup(false),
			ImageResource(name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		std::vector<Unit::Export> getReferences();

		void ready() override;

		TextureType getTextureType() { return TEXTURE_CUBE; }
		
		int getNumImages() { return (int)m_images.size(); }

		Resource* getImage(int index) override;

	protected:
		std::vector<ImageResource*> m_images;
		bool m_setup;
	};
}
#endif // !RHEA_FABRICS_CUBEMAP_H
