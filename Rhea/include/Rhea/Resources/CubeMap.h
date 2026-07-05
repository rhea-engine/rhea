/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_FABRICS_CUBEMAP_H
#define RHEA_FABRICS_CUBEMAP_H

#include "ImageResource.h"

namespace rhea
{
	class CubeMap : public ImageResource
	{
		RECLASS(CubeMap, ImageResource, "//meworla/cubemap")
	public:
		CubeMap(std::string name, std::string type_name) :
			m_setup(false),
			ImageResource(name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		std::vector<Component::Export> getReferences();

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
