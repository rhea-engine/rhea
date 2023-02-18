#include <assert.h> 

#include "Hippios/Renderer.h"
#include "Hippios/D3D11Renderer/D3D11Renderer.h"

#include "Mesh.h"

namespace Poseidon
{
	Mesh::~Mesh()
	{
		if (m_vbuffer)
			delete m_vbuffer;
		if (m_ibuffer)
			delete m_ibuffer;
	}

	void Mesh::setVertices(size_t vertex_size, size_t num_vertices, const void* source)
	{
		assert(m_viewport && "viewport must be initialized before");
		if (m_vbuffer) {			
			if (m_vbuffer->getLength() < vertex_size * num_vertices) {
				delete m_vbuffer;
				m_vbuffer = m_renderer->createVertexBuffer(vertex_size, num_vertices, m_dynamic_buffer);
			}
		}
		else {
			m_vbuffer = m_renderer->createVertexBuffer(vertex_size, num_vertices, m_dynamic_buffer);
		}
		m_vbuffer->WriteData(0, vertex_size * num_vertices, source, false);
		m_vertex_size = (int)vertex_size;
		if (!m_ibuffer) {
			// for non-indexed return vertex count as size
			m_num_indices = num_vertices;
		}
	}

	void Mesh::setIndices(size_t vertex_size, size_t num_faces, const void* source)
	{
		if (m_ibuffer) {
			if (m_ibuffer->getLength() < vertex_size * num_faces * 3) {
				delete m_ibuffer;
				m_ibuffer = m_renderer->createIndexBuffer(vertex_size, num_faces, m_dynamic_buffer);
				m_num_indices = num_faces * 3;
			}
		}
		else {
			m_ibuffer = m_renderer->createIndexBuffer(vertex_size, num_faces, m_dynamic_buffer);
			m_num_indices = num_faces * 3;
		}		
		m_ibuffer->WriteData(0, vertex_size * num_faces * 3, source, false);	
	}

	void Mesh::addVertexBuffer(Hippios::VertexBuffer* vbuffer)
	{
		if (m_vbuffer) {
			delete m_vbuffer;
		}
		m_vbuffer = vbuffer;
	}

	void Mesh::setViewport(Viewport* viewport)
	{
		m_viewport = viewport;		
		m_renderer = m_viewport->get_renderer()->getRenderer();
	}	
}