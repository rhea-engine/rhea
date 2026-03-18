#ifndef HIPPIOS_DEPTHBUFFER_H
#define HIPPIOS_DEPTHBUFFER_H 1

namespace Hippios
{
	class DepthBuffer
	{
	protected:		
		unsigned int m_bit_depth;
		unsigned int m_width;
		unsigned int m_height;
	public:
		DepthBuffer(unsigned int bit_depth, unsigned int width, unsigned int height) :
			m_bit_depth(bit_depth),
			m_width(width),
			m_height(height) {};
		virtual ~DepthBuffer() {}
	};
}

#endif // !HIPPIOS_DEPTHBUFFER_H
