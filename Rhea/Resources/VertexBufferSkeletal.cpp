#include "Rhea/Resources/VertexBufferSkeletal.h"

namespace rhea
{
	void VertexBufferSkeletal::push_back(const VertexSkeletal& vertex)
	{
		VertexBuffer::push_back((float*)&vertex);
	}

	void VertexBufferSkeletal::SetPosition(int index, const math::Vector3& position)
	{
		if (index > m_vertex_count) {
			reserve(index);
		}
		VertexSkeletal* v = (VertexSkeletal*)(m_data + index * m_vertex_size);
		v->position = position;
	}

	void VertexBufferSkeletal::SetNormal(int index, const math::Vector3& normal)
	{
		if (index > m_vertex_count) {
			reserve(index);
		}
		VertexSkeletal* v = (VertexSkeletal*)(m_data + index * m_vertex_size);
		v->normal = normal;
	}

	void VertexBufferSkeletal::SetTexcoord(int index, const math::Vector2& uv)
	{
		if (index > m_vertex_count) {
			reserve(index);
		}
		VertexSkeletal* v = (VertexSkeletal*)(m_data + index * m_vertex_size);
		v->texcoord = uv;
	}

	void VertexBufferSkeletal::SetTangent(int index, const math::Vector3& tangent)
	{
		if (index > m_vertex_count) {
			reserve(index);
		}
		VertexSkeletal* v = (VertexSkeletal*)(m_data + index * m_vertex_size);
		v->tangent = tangent;
	}
}