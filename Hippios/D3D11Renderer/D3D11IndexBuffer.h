#include "Hippios/IndexBuffer.h"
#include "D3D11GraphicsBuffer.h"

#include <memory>

namespace Hippios
{
	class D3D11IndexBuffer : public IndexBuffer
	{	
	public:
		~D3D11IndexBuffer() {}
		D3D11IndexBuffer(ID3D11Device* d3d_device, ID3D11DeviceContext* device_context, size_t index_stride, size_t num_vertices, bool dynamic);
		void* Lock(size_t offset, size_t length);
		void Unlock();
		void ReadData(size_t offset, size_t length, void* dest);
		void WriteData(size_t offset, size_t length, const void* source, bool discardBuffer = false, bool mapped = true);
		ID3D11Buffer* get_buffer();
		DXGI_FORMAT getIndexFormat() const { return m_index_format; }

		bool is_dirty;

	protected:
		std::unique_ptr<D3D11GraphicsBuffer> m_buffer;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_device_context;
		DXGI_FORMAT m_index_format = DXGI_FORMAT_R32_UINT;
	};
}