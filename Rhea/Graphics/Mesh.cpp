#include "../Mesh.h"
#include "../Unit.h"
#include "../ImageResource.h"

#include "Poseidon/Viewport.h"
#include "Poseidon/SceneManager.h"

#include "Mesh.h"
#include "Texture.h"

namespace Rhea {

	void GraphicsMesh::createMesh(Mesh* mesh)
	{		
		Poseidon::Viewport* viewport = m_core->get_scene_manager()->getViewport();

		m_mesh = new Poseidon::Mesh();
		m_mesh->setViewport(viewport);

		// add vertex and index buffer

		std::vector<Mesh::vertex> vertices;
		std::vector<unsigned long> v_indices;
		
		mesh->getBuffers(vertices, v_indices);

		std::vector<Hippios::vertex_uv> hippios_vertices;
		for (auto it = vertices.begin(); it != vertices.end(); ++it) {
			Mesh::vertex v = *it;			
			hippios_vertices.push_back(Hippios::vertex_uv(v.x,v.y,v.z,v.u,v.v));
		}

		m_mesh->setVertices(sizeof(Hippios::vertex_uv), hippios_vertices.size(), hippios_vertices.data());
		m_mesh->setIndices(sizeof(unsigned long), v_indices.size() / 3, v_indices.data());

		// now add the textures
		std::vector<ImageResource*> textures = mesh->getTextures();
		for (auto it = textures.begin(); it != textures.end(); ++it) {
			Element* comp = GraphicsTexture::createElement(m_core, *it);
			Poseidon::Image* tex = ((GraphicsTexture*)comp)->get();
			m_mesh->addTexture((Poseidon::Texture*)tex);
		}

		mesh->addElement("graphics", this);
	}

	Element* GraphicsMesh::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		// create
		GraphicsMesh* mesh = new GraphicsMesh(core);
		mesh->createMesh(static_cast<Mesh*>(unit));
				
		return mesh;
	}
}