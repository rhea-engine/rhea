#include "Hippios/VertexBuffer.h"
#include "D3D11GraphicsBuffer.h"

namespace Hippios
{
	class D3D11VertexBuffer : public VertexBuffer
	{
	protected:	
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_device_context;				
		D3D11GraphicsBuffer* m_buffer;
	public:
		bool is_dirty;
		D3D11VertexBuffer(ID3D11Device* d3d_device, ID3D11DeviceContext* device_context, size_t vertex_size, size_t num_vertices, bool dynamic);
		void* Lock(size_t offset, size_t length);
		void Unlock();
		void ReadData(size_t offset, size_t length, void* dest);
		void WriteData(size_t offset, size_t length, const void* source, bool discard_buffer = false, bool mapped = true);		
		ID3D11Buffer* get_buffer();
	};
}