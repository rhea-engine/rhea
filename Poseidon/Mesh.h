#ifndef POSEIDON_MESH_H
#define POSEIDON_MESH_H

#include "Node.h"
#include "Camera.h"
#include "Viewport.h"
#include "Hippios/Renderer.h"
#include "Hippios/VertexBuffer.h"
#include "Hippios/IndexBuffer.h"
#include "Texture.h"

#include <vector>

namespace Poseidon
{
	class Mesh
	{
	public:
		Mesh() :
			Mesh(false)
		{}
		Mesh(bool dynamic) :
			m_renderer(nullptr),
			m_vbuffer(nullptr),
			m_ibuffer(nullptr),
			m_viewport(nullptr),
			m_vertex_size(0),
			m_num_indices(0),
			m_dynamic_buffer(dynamic)
		{}
		virtual ~Mesh();
		void setVertices(size_t vertex_size, size_t num_vertices, const void* source);
		void setIndices(size_t vertex_size, size_t num_faces, const void* source);
		void setViewport(Viewport* viewport);
		void addVertexBuffer(Hippios::VertexBuffer* vertex_buffer);		

		Hippios::VertexBuffer* get_vbuffer() { return m_vbuffer; };
		Hippios::IndexBuffer* get_ibuffer() { return m_ibuffer; };
		int getNumIndices() { return m_num_indices; }

	protected:
		Hippios::Renderer* m_renderer;
		Hippios::VertexBuffer* m_vbuffer;
		Hippios::IndexBuffer* m_ibuffer;
		Viewport* m_viewport;

		// information about the buffer
		int m_vertex_size;
		int m_num_indices;

		// are we using a dynamic buffer?
		bool m_dynamic_buffer;
	};
}
#endif // !POSEIDON_MESH_H
