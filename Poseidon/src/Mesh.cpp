#include <assert.h> 

#include <Hippios/Renderer.h>

#include "Poseidon/Mesh.h"
#include "Poseidon/Material.h"

namespace poseidon
{
	Mesh::~Mesh()
	{
		// unique_ptrs handle cleanup automatically
	}

	void Mesh::setVertices(size_t vertex_size, size_t num_vertices, const void* source)
	{
		assert(m_viewport && "viewport must be initialized before");
		if (m_vbuffer) {			
			if (m_vbuffer->getLength() < vertex_size * num_vertices) {
				m_vbuffer = m_renderer->createVertexBuffer(vertex_size, num_vertices, m_dynamic_buffer);
			}
		}
		else {
			m_vbuffer = m_renderer->createVertexBuffer(vertex_size, num_vertices, m_dynamic_buffer);
		}
		m_vbuffer->WriteData(0, vertex_size * num_vertices, source, false);
		m_vertex_size = (int)vertex_size;
		m_num_vertices = num_vertices;
	}

	size_t Mesh::addMesh(size_t index_stride, size_t num_faces, const void* source, Material* material)
	{
		SubMesh submesh;
		submesh.material = material;
		submesh.num_indices = num_faces * 3;
		submesh.ibuffer = m_renderer->createIndexBuffer(index_stride, num_faces, m_dynamic_buffer);

		submesh.ibuffer->WriteData(0, index_stride * num_faces * 3, source, false);

		m_submeshes.push_back(std::move(submesh));
		m_num_meshes++;
		return m_submeshes.size() - 1;
	}

	void Mesh::setMesh(size_t mesh_idx, size_t index_stride, size_t num_faces, const void* source, Material* material)
	{
		if (mesh_idx >= m_submeshes.size()) {
			m_submeshes.resize(mesh_idx + 1);
			m_num_meshes = mesh_idx + 1;
		}

		if (m_submeshes[mesh_idx].ibuffer) {
			if (m_submeshes[mesh_idx].ibuffer->getLength() < index_stride * num_faces * 3) {
				m_submeshes[mesh_idx].ibuffer = m_renderer->createIndexBuffer(index_stride, num_faces, m_dynamic_buffer);
				m_submeshes[mesh_idx].num_indices = num_faces * 3;
			}
		}
		else {
			m_submeshes[mesh_idx].ibuffer = m_renderer->createIndexBuffer(index_stride, num_faces, m_dynamic_buffer);
			m_submeshes[mesh_idx].num_indices = num_faces * 3;
		}	
		m_submeshes[mesh_idx].material = material;
		m_submeshes[mesh_idx].ibuffer->WriteData(0, index_stride * num_faces * 3, source, false);
	}

	void Mesh::addVertexBuffer(std::unique_ptr<hippios::VertexBuffer> vertex_buffer)
	{
		m_vbuffer = std::move(vertex_buffer);
	}

	void Mesh::getRenderInput(size_t mesh_idx, hippios::VertexBuffer** pvb, hippios::IndexBuffer** pib, int& num_indices)
	{
		*pvb = get_vbuffer();
		*pib = get_ibuffer(mesh_idx);
		num_indices = (int)getNumIndices(mesh_idx);
	}

	void Mesh::setViewport(Viewport* viewport)
	{
		m_viewport = viewport;		
		m_renderer = m_viewport->get_renderer();
	}	
}