/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_VERTEXBUFFER_SKELETAL_H
#define RHEA_VERTEXBUFFER_SKELETAL_H

#include "VertexBuffer.h"
#include <Math/Vector3.h>
#include <Math/Vector2.h>

namespace rhea
{
	class VertexBufferSkeletal : public VertexBuffer
	{
	public:
		struct VertexSkeletal
		{
			math::Vector3 position;
			math::Vector3 normal;
			math::Vector2 texcoord;
			math::Vector3 tangent;
			// skeletal information
			uint16_t jointIndices[4] = { 0, 0, 0, 0 };
			// Default to no influence
			float weights[4] = { 1.0f, 0.0f, 0.0f, 0.0f }; // Default to full influence from the first joint
		};

		VertexBufferSkeletal() : VertexBuffer(sizeof(VertexSkeletal)) {}

		void push_back(const VertexSkeletal& vertex);

		// get one vertex
		VertexSkeletal* operator[] (size_t index) {
			if (m_data && index >= 0 && index < m_vertex_count)
				return (VertexSkeletal*)&m_data[index * m_vertex_size / sizeof(float)];
			else return nullptr;
		}
		VertexSkeletal* get(size_t index) { return operator[](index); } // alias for operator[](s)

		void SetPosition(int index, const math::Vector3& position);
		void SetNormal(int index, const math::Vector3& normal);
		void SetTexcoord(int index, const math::Vector2& texcoord);
		void SetTangent(int index, const math::Vector3& tangent);
	};
}
#endif // !RHEA_VERTEXBUFFER_SKELETAL_H
