#include "Mesh.h"
#include "Application.h"
#include "ObjLoader.h"
#include "Orion/ResourceStream.h"
#include "UnitTree.h"
#include "Resources/TextResource.h"
#include <Rhea/Fabrics/Core/Shader.h>
#include "Fabrics/Media/ImageResource.h"
#include "../Core/Material.h"
#include "Resources/ResourceList.h"

#include "Graphics/Texture.h"
#include <Psio/Log.h>


namespace Rhea
{
	void Mesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		// dependent resources
		if (m_values.find("texture") != m_values.end() &&
			m_values.find("shader") == m_values.end()) {
			m_values["shader"] = "//rhea/shaders/standard";
		}
	}

	std::vector<Unit::Export> Mesh::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("geometry") != m_values.end())
			refs.push_back({ "geometry", "//meworla/geometryloader", m_values["geometry"] });		
		if (m_values.find("shader") != m_values.end()) {
		//	refs.push_back({ "texture", "//meworla/texture", m_values["texture"] });
			refs.push_back({ "shader", "//meworla/shader", m_values["shader"] });
		}
		return refs;
	}

	void Mesh::ready()
	{
		Unit::ready();

		// using geometry
		if (m_values.find("geometry") != m_values.end()) {
			Geometry* geometry = (Geometry*)m_app->getUnitTree()->find(m_values["geometry"], m_attributes["basepath"], false);
			if (geometry)
				geometry->get(m_vertices, m_v_indices);
		}
		
		if (m_values.find("texture") != m_values.end())
		{
			Unit* tex = nullptr;

			tex = m_app->getUnitTree()->find(m_values["texture"], m_attributes["basepath"], false);
			if (tex) {
				if (!m_material) {
					m_material = new Material(m_path + "_material");
				}
				m_material->setup(m_app);
				m_material->addTexture(static_cast<ImageResource*>(tex));
				m_material->setValue("shader", getValue("shadername", "standard"));
				if (m_values.find("shadername") != m_values.end()) {
					LOG(LOG_DEBUG) << "shadername set: " << (std::string)m_values["shadername"];
				}

				// get the shader				
				Unit* shader_res = m_app->getUnitTree()->find(m_values["shader"], m_attributes["basepath"], false);
				if (shader_res) {
					m_material->setShaderRes(doCast<Shader>(shader_res));
				}
				else {
					LOG(LOG_ERROR) << "Could not find shader resource: " << (std::string)m_values["shader"];
				}
			}
		}	
		if (m_values.find("material") != m_values.end()) {
			m_material = doCast<Material>(m_app->getUnitTree()->find(m_values["material"], m_attributes["basepath"], false));
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
}