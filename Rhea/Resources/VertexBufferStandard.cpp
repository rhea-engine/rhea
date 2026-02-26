#include "Rhea/Resources/VertexBufferStandard.h"

namespace Rhea
{	
	void VertexBufferStandard::push_back(const VertexStandard& vertex)
	{
		VertexBuffer::push_back((float*)&vertex);
	}

	void VertexBufferStandard::SetPosition(int index, const Math::Vector3& position)
	{
		if (index > m_vertex_count) {
			reserve(index);
		}
		VertexStandard* v = (VertexStandard*)(m_data + index * m_vertex_size);
		v->position = position;
	}

	void VertexBufferStandard::SetNormal(int index, const Math::Vector3& normal)
	{
		if (index > m_vertex_count) {
			reserve(index);
		}
		VertexStandard* v = (VertexStandard*)(m_data + index * m_vertex_size);
		v->normal = normal;
	}

	void VertexBufferStandard::SetTexcoord(int index, const Math::Vector2& uv)
	{
		if (index > m_vertex_count) {
			reserve(index);
		}
		VertexStandard* v = (VertexStandard*)(m_data + index * m_vertex_size);
		v->texcoord = uv;
	}

	void VertexBufferStandard::SetTangent(int index, const Math::Vector3& tangent)
	{
		if (index > m_vertex_count) {
			reserve(index);
		}
		VertexStandard* v = (VertexStandard*)(m_data + index * m_vertex_size);
		v->tangent = tangent;
	}
}