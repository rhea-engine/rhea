#ifndef HIPPIOS_VERTEXBUFFER_H
#define HIPPIOS_VERTEXBUFFER_H

#include "GraphicsBuffer.h"

namespace Hippios
{
	struct D3DXCOLOR {
		float a, b, c, d;
	};
	// a struct to define a single vertex
	struct vertex { float X, Y, Z; D3DXCOLOR Color; };
	struct vertex_uv {
		float x, y, z, u, v;
		vertex_uv() {}
		vertex_uv(float _x, float _y, float _z,
			float _u, float _v) : x(_x), y(_y), z(_z), u(_u), v(_v) {}
	};

	typedef std::vector<vertex> vertexVec;
	typedef std::vector<vertex_uv> vertexUVVec;

	class VertexBuffer : public GraphicsBuffer
	{
	protected:
		size_t m_vertex_size;
		size_t m_num_vertices;
	public:		
		size_t get_vertex_size() { return m_vertex_size; };
		size_t get_vertex_count() { return m_num_vertices; };
		void update(const void* source) {}
	};
}
#endif // !HIPPIOS_VERTEXBUFFER_H
