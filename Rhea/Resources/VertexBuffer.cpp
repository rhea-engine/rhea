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

		// Anzahl floats pro Vertex
		size_t floats_per_vertex = m_vertex_size / sizeof(float);
		
		for (size_t i = 0; i < floats_per_vertex; i++) {
			m_data[m_vertex_count * floats_per_vertex + i] = vertex[i];
		}
		m_vertex_count++;
	}

	void VertexBuffer::reserve(size_t num_elements)
	{
		// Berechne benötigte Größe in float-Elementen
		size_t floats_per_vertex = m_vertex_size / sizeof(float);
		size_t required_floats = num_elements * floats_per_vertex;
		size_t current_floats = m_buf_size / sizeof(float);
		
		if (required_floats <= current_floats) {
			return;
		}

		// Reserve mit Padding (in Vertices, nicht Bytes)
		size_t padded_vertices = (num_elements + 63) / 64 * 64 + 64;
		size_t new_float_count = padded_vertices * floats_per_vertex;
		
		float* new_data = new float[new_float_count];

		if (m_data) {
			// Kopiere alte Daten (in float-Elementen)
			size_t old_float_count = m_buf_size / sizeof(float);
			std::memcpy(new_data, m_data, old_float_count * sizeof(float));
			delete[] m_data;
		}

		m_data = new_data;
		m_buf_size = new_float_count * sizeof(float);  // In Bytes für Konsistenz
	}
}