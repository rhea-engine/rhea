#include "Hippios/IndexBuffer.h"
#include "D3D11GraphicsBuffer.h"

namespace Hippios
{
	class D3D11IndexBuffer : public IndexBuffer
	{
	protected:
		D3D11GraphicsBuffer* m_buffer;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_device_context;
	public:
		bool is_dirty;
		D3D11IndexBuffer(ID3D11Device* d3d_device, ID3D11DeviceContext* device_context, size_t vertex_size, size_t num_vertices);
		void* Lock(size_t offset, size_t length);
		void Unlock();
		void ReadData(size_t offset, size_t length, void* dest);
		void WriteData(size_t offset, size_t length, const void* source, bool discardBuffer = false, bool mapped = true);
		ID3D11Buffer* get_buffer();
	};
}