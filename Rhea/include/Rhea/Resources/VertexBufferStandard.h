#ifndef RHEA_VERTEXBUFFER_STANDARD_H
#define RHEA_VERTEXBUFFER_STANDARD_H

#include "VertexBuffer.h"
#include <Math/Vector3.h>
#include <Math/Vector2.h>

namespace rhea
{
	class VertexBufferStandard : public VertexBuffer
	{
	public:
		struct VertexStandard
		{
			math::Vector3 position;
			math::Vector3 normal;
			math::Vector2 texcoord;
			math::Vector3 tangent;
		};

		VertexBufferStandard() : VertexBuffer(sizeof(VertexStandard)) {}

		void push_back(const VertexStandard& vertex);

		// get one vertex
		VertexStandard* operator[] (size_t index) { 
			if (m_data && index >= 0 && index < m_vertex_count) 
				return (VertexStandard*)&m_data[index * m_vertex_size / sizeof(float)];
			else return nullptr; 
		}
		VertexStandard* get(size_t index) { return operator[](index); } // alias for operator[](s)

		void SetPosition(int index, const math::Vector3& position);
		void SetNormal(int index, const math::Vector3& normal);
		void SetTexcoord(int index, const math::Vector2& texcoord);
		void SetTangent(int index, const math::Vector3& tangent);
	};
}
#endif // !RHEA_VERTEXBUFFER_STANDARD_H
