#include "Rectangle2D.h"

namespace Poseidon
{
	Rectangle2D::~Rectangle2D()
	{
		Mesh::~Mesh();
	}
	
	void Rectangle2D::setDimension(float x, float y, float w, float h)
	{
		m_x = x;
		m_y = y;
		m_w = w;
		m_h = h;
	}

	void Rectangle2D::setTextureCoords(float u1, float v1, float u2, float v2)
	{
		m_u1 = u1;
		m_v1 = v1;
		m_u2 = u2;
		m_v2 = v2;
	}
	
	void Rectangle2D::createBuffers()
	{
		if (!m_renderer)
		{
			printf("Rectangle2D::createBuffers: renderer not set!");
			return;
		}

		float z = 0.0f;
		std::vector<Hippios::vertex_uv> vertices = 
		{ 
			{ m_x, m_y, z, m_u1, m_v2 }, // bottom-left
			{ m_x, m_y + m_h, z, m_u1, m_v1 }, // top-left
			{ m_x + m_w, m_y + m_h, z, m_u2, m_v1 }, // top-right
			{ m_x + m_w, m_y, z, m_u2, m_v2 }  // bottom-right
		};
		std::vector<unsigned long> v_indices = { 0, 1, 2, 0, 2, 3 };

		// vertex buffer
		setVertices(sizeof(Hippios::vertex_uv), vertices.size(), vertices.data());

		// index buffer
		setIndices(sizeof(unsigned long), v_indices.size() / 3, v_indices.data());
	}
}
