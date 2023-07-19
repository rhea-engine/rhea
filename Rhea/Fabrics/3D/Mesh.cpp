#include "Mesh.h"
#include "Application.h"
#include "ObjLoader.h"
#include "Orion/ResourceStream.h"
#include "UnitTree.h"
#include "Resources/TextResource.h"
#include "Fabrics/Media/ImageResource.h"
#include "../Core/Material.h"
#include "Resources/ResourceList.h"

#include "Graphics/Texture.h"


namespace Rhea
{
	void Mesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		// dependent resources
		if (m_values.find("texture") != m_values.end() &&
			m_values.find("shader") == m_values.end()) {
			m_values["shader"] = "//rhea/shaders/uvshader_vp.hlsl";
		}
	}

	std::vector<Unit::Export> Mesh::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("geometry") != m_values.end())
			refs.push_back({ "geometry", "//meworla/geometryloader", m_values["geometry"] });		
		if (m_values.find("texture") != m_values.end()) {
			refs.push_back({ "texture", "//meworla/texture", m_values["texture"] });
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
			geometry->get(m_vertices, m_v_indices);
		}
		
		if (m_values.find("texture") != m_values.end())
		{
			Unit* tex = nullptr;

			tex = m_app->getUnitTree()->find(m_values["texture"], m_attributes["basepath"], false);
			if (tex) {
				m_material = new Material(m_path + "_material");
				m_material->setup(m_app);
				m_material->addTexture(static_cast<ImageResource*>(tex));
				m_material->setValue("shader", "uvshader_vp");

				// get the shader
				TextResource* shader_res = static_cast<TextResource*>(m_app->getUnitTree()->find(m_values["shader"], m_attributes["basepath"], false));
				if (shader_res) {				
					m_material->setShader(shader_res);
				}
				else {
					//abort();
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