#ifndef HIPPIOS_INDEXBUFFER_H
#define HIPPIOS_INDEXBUFFER_H

#include <vector>
#include "GraphicsBuffer.h"

namespace Hippios
{
	class IndexBuffer : public GraphicsBuffer
	{
	protected:
		size_t m_vertex_size;
		size_t m_num_faces;
	public:
		size_t get_index_count() { return m_num_faces*3; };
	};
}
#endif // !HIPPIOS_IndexBUFFER_H
