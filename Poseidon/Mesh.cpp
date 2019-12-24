#include <assert.h> 

#include "Hippios/Renderer.h"

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
	}

	void Mesh::addVertexBuffer(Hippios::VertexBuffer* vbuffer)
	{
		m_vbuffer = vbuffer;
	}

	void Mesh::setViewport(Viewport* viewport)
	{
		viewport->addRenderChild(this);
		m_viewport = viewport;		
		m_renderer = m_viewport->get_renderer()->getRenderer();
	}

	bool Mesh::render(Camera* camera)
	{
		if (!m_vbuffer)
			return false;

		// update scene
		rot += .0005f;
		if (rot > 6.26f)
			rot = 0.0f;
		cube1World = XMMatrixIdentity();
		//Define cube1's world space matrix
		XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		Rotation = XMMatrixRotationAxis(rotaxis, rot);
		Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);

		//Set cube1's world space using the transformations
		cube1World = Translation * Rotation;

		camera->render(cube1World);

		// render our vertex buffer
		// return true (= quit) on failure
		if (!m_renderer->drawBuffer(m_vbuffer, m_ibuffer, m_texture->get_texture()))
			return false;
		else
			return true;
	}

	void Mesh::addTexture(Texture* texture)
	{
		m_texture = texture;
	}
}