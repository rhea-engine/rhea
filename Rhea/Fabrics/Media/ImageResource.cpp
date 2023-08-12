#include "ImageResource.h"
#include "UnitTree.h"
#include "Resources/ResourceList.h"

#include "Application.h"
#include "Orion/ResourceStream.h"

namespace Rhea
{
	ImageResource::~ImageResource()
	{		
	}

	void ImageResource::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);		
	}

	std::vector<Unit::Export> ImageResource::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("path") != m_values.end()) {
			refs.push_back({ "image", "//meworla/image", m_values["path"] });
		}
		return refs;
	}

	void ImageResource::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		auto path_it = m_values.find("path");
		if (!m_buf && path_it != m_values.end()) {
			ImageResource* img = static_cast<ImageResource*>(m_app->getUnitTree()->find(path_it->second, m_attributes["basepath"], false));
			if (img && img->m_buflen) {
				m_buf = (char*)malloc(img->m_buflen);
				if (img->m_buf != nullptr) {
					memcpy(m_buf, img->m_buf, img->m_buflen);
					m_buflen = img->m_buflen;
				}
			}
		}		
	}
}
