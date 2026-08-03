/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_VERTEXBUFFER_COLOR_H
#define RHEA_VERTEXBUFFER_COLOR_H

#include "VertexBuffer.h"

#include <Math/Vector3.h>
#include <Math/Vector4.h>

#include <cstring>

namespace rhea
{
	class VertexBufferColor : public VertexBuffer
	{
	public:
		struct VertexColor
		{
			math::Vector3 position;
			math::Vector4 color;
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

		// update alle Vertices im Buffer
		void updateVertices(const VertexColor* vertices, size_t count)
		{
			reserve(count);
			memcpy(m_data, vertices, count * sizeof(VertexColor));
			m_vertex_count = count;
		}
	};
}
#endif // !RHEA_VERTEXBUFFER_COLOR_H
