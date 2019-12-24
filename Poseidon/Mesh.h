#ifndef POSEIDON_MESH_H
#define POSEIDON_MESH_H

#include "Node.h"
#include "Camera.h"
#include "Viewport.h"
#include "Hippios/Renderer.h"
#include "Hippios/VertexBuffer.h"
#include "Hippios/IndexBuffer.h"
#include "Texture.h"
#include <DirectXMath.h>
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

namespace Poseidon
{
	class Mesh : public Node
	{
	protected:
		Hippios::Renderer* m_renderer = 0;
		Hippios::VertexBuffer* m_vbuffer = 0;
		Hippios::IndexBuffer* m_ibuffer = 0;
		Viewport* m_viewport = 0;
		int m_vertex_size = 0;	
		Texture* m_texture = 0;
		XMMATRIX cube1World;
		XMMATRIX Rotation;
		XMMATRIX Scale;
		XMMATRIX Translation;
		float rot = 0.01f;
	public:		
		void setVertices(size_t vertex_size, size_t num_vertices, const void* source);
		void setIndices(size_t vertex_size, size_t num_faces, const void* source);
		void setViewport(Viewport* viewport);
		void addVertexBuffer(Hippios::VertexBuffer* vertex_buffer);		
		bool render(Camera* camera);
		void addTexture(Texture* texture);

		Hippios::VertexBuffer* get_vbuffer() { return m_vbuffer; };
		Hippios::IndexBuffer* get_ibuffer() { return m_ibuffer; };
	};
}
#endif // !POSEIDON_MESH_H
