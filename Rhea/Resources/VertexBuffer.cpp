/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Resources/VertexBuffer.h"

#include <cstring>

namespace rhea
{
	VertexBuffer::VertexBuffer(const VertexBuffer& vb) :
		m_vertex_size(vb.m_vertex_size),
		m_vertex_count(vb.m_vertex_count),
		m_buf_size(vb.m_buf_size)
	{
		// copy the data
		m_data = new float[m_buf_size];
		memcpy(m_data, vb.m_data, m_buf_size);
	}

	VertexBuffer::~VertexBuffer()
	{
		if (m_data) {
			delete[] m_data;
		}
	}

	void VertexBuffer::push_back(float* vertex)
	{
		reserve(m_vertex_count + 1);

		for (int i = 0; i < (m_vertex_size / sizeof(float)); i++) {
			m_data[m_vertex_count * (m_vertex_size / sizeof(float)) + i] = vertex[i];
		}
		m_vertex_count++;
	}

	void VertexBuffer::reserve(size_t num_elements)
	{
		if (num_elements * m_vertex_size <= m_buf_size) {
			return;
		}

		// reserve some extra space
		num_elements = (num_elements + 63) / 64 * 64 + 64;

		size_t new_size = num_elements * m_vertex_size;
		float* new_data = new float[new_size];

		if (m_data) {
			memcpy(new_data, m_data, m_buf_size);
			delete[] m_data;
		}

		m_data = new_data;
		m_buf_size = new_size;
	}
}