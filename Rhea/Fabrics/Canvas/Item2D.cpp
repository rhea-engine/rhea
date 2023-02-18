#include "Item2D.h"
#include "Application.h"
#include "Fabrics/Media/ImageResource.h"
#include "TextResource.h"
#include "UnitTree.h"
#include "ResourceList.h"
#include <Psio/Log.h>

namespace Rhea
{
	void Item2D::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		m_status = Status::SETUP;

		x = values["x"];
		y = values["y"];		
		w = values["w"];
		h = values["h"];

		m_resource_list = new ResourceList(app);

		if (m_values.find("texture") != m_values.end()) {
			m_resource_list->push("texture", m_values["texture"], "//meworla/texture");
			if (m_values.find("shader") == m_values.end()) {
				// set a default
				m_values["shader"] = "//rhea/shaders/widget.hlsl";
			}
			m_resource_list->push("shader", m_values["shader"], "//meworla/shader");
		}
	}

	Unit* Item2D::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Item2D(name, type);
		return ret;
	}

	void Item2D::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		// load and add textures
		if (m_values.find("texture") != m_values.end())
		{
			Unit* tex = (*m_resource_list)["texture"];
			if (tex) {

				// create dummy material
				m_material = new Material(m_path + "_material");
				m_material->setup(m_app);
				m_material->addTexture(static_cast<ImageResource*>(tex));
				m_material->setValue("shader", "widget");

				// get the shader
				Unit* shader_res = (*m_resource_list)["shader"];
				if (shader_res) {
					m_material->setShader(static_cast<TextResource*>(shader_res));
				}
				else {
					LOG(LOG_ERROR) << "item2d::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
				}
			}
			else {
				LOG(LOG_ERROR) << "item2d::ready(): texture '" << (std::string)m_values["texture"] << "' for " << m_pathname << " not found";
			}
		}
	}
}
