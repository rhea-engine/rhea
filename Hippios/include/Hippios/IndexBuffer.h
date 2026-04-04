#ifndef HIPPIOS_INDEXBUFFER_H
#define HIPPIOS_INDEXBUFFER_H

#include <vector>
#include "GraphicsBuffer.h"

namespace hippios
{
	class IndexBuffer : public GraphicsBuffer
	{
	protected:
		size_t m_index_stride;
		size_t m_num_faces;
	public:
		size_t get_index_count() { return m_num_faces*3; };
		void update(const void* source) {}
		size_t getIndexSize() const { return m_index_stride; }
	};
}
#endif // !HIPPIOS_IndexBUFFER_H
