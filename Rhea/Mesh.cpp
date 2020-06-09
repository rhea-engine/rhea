#include "Mesh.h"
#include "Application.h"
#include "Importer.h"
#include "Orion/ResourceStream.h"
#include "UnitTree.h"
#include "TextResource.h"
#include "ImageResource.h"
#include "Material.h"

#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"

namespace Rhea
{
	void Mesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		m_app = app;
		m_values = values;
		m_attributes = attributes;		
	}

	void Mesh::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			handler(ReqError::OK);

		// load all dependend resources

		std::string mtlbuf;
		std::string objbuf;

		m_app->loadResource(m_values["material"], "//meworla/wavefront-material", [&](Unit* unit, ReqError err) {
			if (unit) {
				mtlbuf = ((TextResource*)unit)->getAsString();
			}
			if (m_values["path"] == "") {
				m_status = Status::CONFIG_ERR;
				return handler(ReqError::CONFIG_ERR);
			}
			m_app->loadResource(m_values["path"], "//meworla/wavefront-object", [&](Unit* unit, ReqError err) {
				if (unit) {
					m_status = Status::LOADED;
					objbuf = ((TextResource*)unit)->getAsString();
					Importer importer;
					importer.load(m_app, objbuf, mtlbuf, m_vertices, m_v_indices);
					// load and add textures
					if (m_values["texture"] != "")
					{
						m_app->loadResource(m_values["texture"], "//meworla/texture", [&](Unit* unit, ReqError err) {
							if (unit) {
								// create dummy material
								m_material = new Material(m_path + "_material");
								m_material->setup(m_app);
								m_material->addTexture(static_cast<ImageResource*>(unit));
								m_material->setValue("shader", "uvshader_vp");

								// get the shader
								Unit* shader_res = m_app->getUnitTree()->find("//rhea/shaders/uvshader_vp.hlsl");
								if (shader_res) {
									// already loaded
									m_material->setShader(static_cast<TextResource*>(shader_res));
									handler(ReqError::OK);
								}
								else {
									// load the shader
									m_app->loadResource("//rhea/shaders/uvshader_vp.hlsl", "//meworla/shader", [&](Unit* unit, ReqError err) {
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
				else {
					m_status = Status::NOT_FOUND;
					handler(err);
				}
				});
			});
	}

	void Mesh::getBuffers(std::vector<vertex>& vertices, std::vector<unsigned long>& indices)
	{
		vertices = m_vertices;
		indices = Rhea::Mesh::m_v_indices;
	}

	void Mesh::setBuffers(std::vector<vertex>& vertices, std::vector<unsigned long>& indices)
	{
		m_vertices = vertices;
		m_v_indices = indices;
	}

	Unit* Mesh::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Mesh(name);
		return ret;
	}
}