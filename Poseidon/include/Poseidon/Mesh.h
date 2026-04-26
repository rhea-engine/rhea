#ifndef POSEIDON_MESH_H
#define POSEIDON_MESH_H

#include "Camera.h"
#include "Viewport.h"
#include "Hippios/Renderer.h"
#include "Hippios/VertexBuffer.h"
#include "Hippios/IndexBuffer.h"
#include "Texture.h"
#include "Material.h"

#include <vector>

namespace poseidon
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
			m_viewport(nullptr),
			m_vertex_size(0),
			m_num_vertices(0),
			m_num_meshes(0),
			m_dynamic_buffer(dynamic),
			m_indexed(true)
		{}
		virtual ~Mesh();
		
		struct SubMesh
		{
			std::unique_ptr<hippios::IndexBuffer> ibuffer;
			size_t num_indices = 0;
			Material* material = nullptr;
		};
		
		// vertices

		void setVertices(size_t vertex_size, size_t num_vertices, const void* source);		
		void setViewport(Viewport* viewport);
		void addVertexBuffer(std::unique_ptr<hippios::VertexBuffer> vertex_buffer);
		
		void getRenderInput(size_t mesh_idx, hippios::VertexBuffer** pvb, hippios::IndexBuffer** pib, int& num_indices);
		hippios::VertexBuffer* get_vbuffer() { return m_vbuffer.get(); };
		// return number of vertices
		size_t getNumVertices() { return m_num_vertices; }

		// set mesh
		size_t addMesh(size_t index_stride, size_t num_faces, const void* source, Material* material);
		void setMesh(size_t mesh_idx, size_t index_stride, size_t num_faces, const void* source, Material* material);

		// get meshes
		const size_t getMeshCount() { return m_num_meshes; }
		hippios::IndexBuffer* get_ibuffer(size_t mesh_idx) { if (mesh_idx < m_num_meshes) return m_submeshes[mesh_idx].ibuffer.get(); else return nullptr; };
		size_t getNumIndices(size_t mesh_idx) { if (mesh_idx < m_num_meshes) return m_submeshes[mesh_idx].num_indices; else return 0; }
		SubMesh* getSubMesh(size_t mesh_idx) { if (mesh_idx < m_num_meshes) return &m_submeshes[mesh_idx]; else return nullptr; }

		// get if the buffer is indexed
		bool isIndexed() { return m_indexed; }
		// set if the buffer is indexed
		void setIndexed(bool indexed) { m_indexed = indexed; }



	protected:
		hippios::Renderer* m_renderer;
		std::unique_ptr<hippios::VertexBuffer> m_vbuffer;
		std::vector<SubMesh> m_submeshes;
		Viewport* m_viewport;	

		// information about the vertex buffer
		size_t m_vertex_size;
		size_t m_num_vertices;
		bool m_indexed;
		size_t m_num_meshes;

		// are we using a dynamic buffer?
		bool m_dynamic_buffer;
	};
}
#endif // !POSEIDON_MESH_H
