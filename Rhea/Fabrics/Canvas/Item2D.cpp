#include "Item2D.h"
#include "Application.h"
#include "ImageResource.h"
#include "TextResource.h"
#include "UnitTree.h"
#include "ResourceList.h"

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
			m_resource_list->push("shader", "//rhea/shaders/widget.hlsl", "//meworla/shader");
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
			Unit* tex = m_app->getUnitTree()->find(m_values["texture"]);
			if (tex) {

				// create dummy material
				m_material = new Material(m_path + "_material");
				m_material->setup(m_app);
				m_material->addTexture(static_cast<ImageResource*>(tex));
				m_material->setValue("shader", "widget");

				// get the shader
				Unit* shader_res = m_app->getUnitTree()->find("//rhea/shaders/widget.hlsl");
				if (shader_res) {
					m_material->setShader(static_cast<TextResource*>(shader_res));
				}
				else {
					m_app->log("item2d::ready(): Shader for " + m_pathname + " not found");
				}
			}
			else {
				m_app->log("item2d::ready(): texture for " + m_pathname + " not found");
			}
		}
	}
}
