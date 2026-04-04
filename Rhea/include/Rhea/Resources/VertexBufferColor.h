#ifndef RHEA_VERTEXBUFFER_COLOR_H
#define RHEA_VERTEXBUFFER_COLOR_H

#include "VertexBuffer.h"

#include <Components/Math/Vector3.h>

namespace rhea
{
	class VertexBufferColor : public VertexBuffer
	{
	public:
		struct VertexColor
		{
			Math::Vector3 position;
			Math::Vector3 color;
		};

		VertexBufferColor() : VertexBuffer(sizeof(VertexColor)) {}

		// get one vertex
		VertexColor* operator[] (size_t index) { if (m_data && index >= 0 && index < m_vertex_count) return (VertexColor*)m_data + index * m_vertex_size; else return nullptr; }

		// add one vertex
		void push_back(const VertexColor& vertex)
		{
			reserve(m_vertex_count + 1);
			memcpy(m_data + m_vertex_count * m_vertex_size, &vertex, m_vertex_size);
			m_vertex_count++;
		}
	};
}
#endif // !RHEA_VERTEXBUFFER_COLOR_H
