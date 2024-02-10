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
			m_dynamic_buffer(dynamic),
			m_indexed(true)
		{}
		virtual ~Mesh();
		void setVertices(size_t vertex_size, size_t num_vertices, const void* source);
		void setIndices(size_t vertex_size, size_t num_faces, const void* source);
		void setViewport(Viewport* viewport);
		void addVertexBuffer(Hippios::VertexBuffer* vertex_buffer);		
		void getRenderInput(Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib, int& num_indices);

		Hippios::VertexBuffer* get_vbuffer() { return m_vbuffer; };
		Hippios::IndexBuffer* get_ibuffer() { return m_ibuffer; };
		size_t getNumIndices() { return m_num_indices; }

		// get if the buffer is indexed
		bool isIndexed() { return m_indexed; }
		// set if the buffer is indexed
		void setIndexed(bool indexed) { m_indexed = indexed; }

	protected:
		Hippios::Renderer* m_renderer;
		Hippios::VertexBuffer* m_vbuffer;
		Hippios::IndexBuffer* m_ibuffer;
		Viewport* m_viewport;

		// information about the buffer
		size_t m_vertex_size;
		size_t m_num_indices;
		bool m_indexed;

		// are we using a dynamic buffer?
		bool m_dynamic_buffer;
	};
}
#endif // !POSEIDON_MESH_H
