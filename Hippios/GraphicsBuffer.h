#ifndef HIPPIOS_GRAPHICSBUFFER_H
#define HIPPIOS_GRAPHICSBUFFER_H

#include <vector>

namespace Hippios
{		
	class GraphicsBuffer
	{
	protected:
		bool m_is_locked = false;
		size_t m_size_in_bytes = 0;
	public:
		virtual ~GraphicsBuffer() = default;
		virtual void* Lock(size_t offset, size_t length) {
			m_is_locked = true;
			return 0;
		};
		virtual void Unlock() {
			m_is_locked = false;
		};
		virtual void ReadData(size_t offset, size_t length, void* dest) = 0;
		virtual void WriteData(size_t offset, size_t length, const void* source, bool discardBuffer = false, bool mapped = true) = 0;
		virtual void update(const void* source) = 0;
		size_t getLength() { return m_size_in_bytes; }
	};
}
#endif // !HIPPIOS_GRAPHICSBUFFER_H
