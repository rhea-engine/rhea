#include "Item2D.h"
#include "Application.h"
#include "ImageResource.h"
#include "TextResource.h"
#include "UnitTree.h"

namespace Rhea
{
	void Item2D::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_attributes = attributes;
		m_values = values;
		m_app = app;
		m_status = Status::SETUP;

		x = values["x"];
		y = values["y"];
		w = values["w"];
		h = values["h"];
	}

	void Item2D::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			return handler(ReqError::OK);

		if (m_status == Status::SETUP) {
			loadDependencies(handler);
		}
		else {
			// TODO load myself and then call dependencies
			printf("Item2D::load without setup not implemented\n");
		}
	}

	Unit* Item2D::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Item2D(name, type);
		return ret;
	}

	void Item2D::loadDependencies(ReadyHandler handler)
	{
		// load and add textures
		if (m_values["texture"] != "")
		{
			m_app->loadResource(m_values["texture"], "//meworla/texture", [&](Unit* unit, ReqError err) {
				if (unit) {

					// create dummy material
					m_material = new Material(m_path + "_material");
					m_material->setup(m_app);
					m_material->addTexture(static_cast<ImageResource*>(unit));
					m_material->setValue("shader", "widget");

					// get the shader
					Unit* shader_res = m_app->getUnitTree()->find("//rhea/shaders/widget.hlsl");
					if (shader_res) {
						// already loaded
						m_material->setShader(static_cast<TextResource*>(shader_res));
						handler(ReqError::OK);
					}
					else {
						// load the shader
						m_app->loadResource("//rhea/shaders/widget.hlsl", "//meworla/shader", [&](Unit* unit, ReqError err) {
							if (unit) {
								m_material->setShader(static_cast<TextResource*>(unit));
								handler(ReqError::OK);
							}
							else {
								handler(ReqError::RESOURCE_NOT_FOUND);
							}
							});
					}
				}
				else
					handler(err);
				});
		}
		else
			handler(ReqError::OK);
	}
}
