#ifndef RHEA_VERTEX_BUFFER_H
#define RHEA_VERTEX_BUFFER_H

#include <string>

namespace Rhea
{
	// a generic vertex buffer
	class VertexBuffer
	{
	public:
		VertexBuffer() :
			VertexBuffer(0)
		{}
		VertexBuffer(size_t vertex_size) :
			m_data(nullptr),
			m_vertex_size(vertex_size),
			m_vertex_count(0),
			m_buf_size(0)
		{}

		~VertexBuffer();

		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		VertexBuffer(VertexBuffer&&) = default;
		VertexBuffer& operator=(VertexBuffer&&) = default;

		// get vertex dimensions

		// get vertex size in bytes
		size_t getVertexSize() const { return m_vertex_size; }
		// get number of vertices
		size_t size() { return m_vertex_count; }

		// set dimension
		void setVertexSize(size_t size) { m_vertex_size = size; }

		// get one vertex
		float* operator[] (size_t index) { if (m_data && index >= 0 && index < m_vertex_count) return &m_data[index * m_vertex_size / sizeof(float)]; else return nullptr; }
		
		void push_back(float* vertex);

		// add a vertex
		template <typename... Args>
		void append(Args... args) {
			void reserver(m_vertex_count + 1);
			constexpr std::size_t num_elems = sizeof...(Args);
			addVertexHelper(0, args...);
			m_vertex_count++;
		}

		// reserve buffer for number of elements
		void reserve(size_t num_elements);

		// get data pointer
		void* data() { return m_data; }

	protected:
		// the actual vertex buffer
		float* m_data;

		size_t m_vertex_size;  // size of one vertex in bytes
		size_t m_vertex_count; // number of vertices in the buffer

		size_t m_buf_size;     // size of the buffer in number of vertices

		// add helpers
		template <typename T, typename... Args>
		void addVertexHelper(size_t index, T arg, Args... args) {
			if (index >= m_vertex_size)
				return;
			m_data[m_vertex_count * m_vertex_size + index] = arg;
			addVertexHelper(index + 1, args...);
		}
		void addVertexHelper(size_t index) {}
	};
}
#endif // !RHEA_VERTEX_BUFFER_H
