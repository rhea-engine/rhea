#include "Mesh.h"
#include "Application.h"
#include "Importer.h"
#include "Orion/ResourceStream.h"
#include "UnitTree.h"
#include "TextResource.h"
#include "ImageResource.h"
#include "Material.h"
#include "ResourceList.h"

#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"

namespace Rhea
{
	void Mesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		m_resource_list = new ResourceList(app);

		// dependent resources
		m_resource_list->push("material", m_values["material"], "//meworla/wavefront-material");

		m_resource_list->push("obj", m_values["path"], "//meworla/wavefront-object");
		if (m_values.find("texture") != m_values.end()) {
			m_resource_list->push("texture", m_values["texture"], "//meworla/texture");
			m_resource_list->push("shader", "//rhea/shaders/uvshader_vp.hlsl", "//meworla/shader");
		}
	}

	void Mesh::ready()
	{
		if (m_status == Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		std::string mtlbuf;
		std::string objbuf;

		// add material
		Unit* mtl = m_app->getUnitTree()->find(m_values["material"]);
		if (mtl) {
			mtlbuf = ((TextResource*)mtl)->getAsString();
		}

		// add object from .objfile
		Unit *obj = m_app->getUnitTree()->find(m_values["path"]);
		if (obj) {
			objbuf = ((TextResource*)obj)->getAsString();
			Importer importer;
			importer.load(m_app, objbuf, mtlbuf, m_vertices, m_v_indices);
		}
		
		if (m_values.find("texture") != m_values.end())
		{
			Unit* tex = m_app->getUnitTree()->find(m_values["texture"]);
			if (tex) {
				m_material = new Material(m_path + "_material");
				m_material->setup(m_app);
				m_material->addTexture(static_cast<ImageResource*>(tex));
				m_material->setValue("shader", "uvshader_vp");

				// get the shader
				Unit* shader_res = m_app->getUnitTree()->find("//rhea/shaders/uvshader_vp.hlsl");
				if (shader_res) {				
					m_material->setShader(static_cast<TextResource*>(shader_res));
				}
			}
		}	
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