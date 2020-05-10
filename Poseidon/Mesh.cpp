#include <assert.h> 

#include "Hippios/Renderer.h"
#include "Hippios/D3D11Renderer/D3D11Renderer.h"

#include "Mesh.h"

using namespace DirectX;

namespace Poseidon
{
	void Mesh::setVertices(size_t vertex_size, size_t num_vertices, const void* source)
	{
		assert(m_viewport && "viewport must be initialized before");
		m_vbuffer = m_renderer->createVertexBuffer(vertex_size, num_vertices);
		m_vbuffer->WriteData(0, vertex_size * num_vertices, source, false);
		m_vertex_size = (int)vertex_size;
	}

	void Mesh::setIndices(size_t vertex_size, size_t num_faces, const void* source)
	{
		m_ibuffer = m_renderer->createIndexBuffer(vertex_size, num_faces);
		m_ibuffer->WriteData(0, vertex_size * num_faces * 3, source, false);	
		m_num_indices = num_faces * 3;
	}

	void Mesh::addVertexBuffer(Hippios::VertexBuffer* vbuffer)
	{
		m_vbuffer = vbuffer;
	}

	void Mesh::setViewport(Viewport* viewport)
	{
		m_viewport = viewport;		
		m_renderer = m_viewport->get_renderer()->getRenderer();
	}

	void Mesh::addTexture(Texture* texture)
	{
		m_texture = texture;
	}
}