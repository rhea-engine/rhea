#ifndef D3D11GRAPHICSBUFFER_H
#define D3D11GRAPHICSBUFFER_H 1

#include "Hippios/GraphicsBuffer.h"

#include <d3d11.h>

namespace Hippios
{
	class D3D11GraphicsBuffer : public GraphicsBuffer
	{
	protected:
		ID3D11Buffer* m_buffer;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_device_context;		
		bool m_dynamic;

	public:
		D3D11GraphicsBuffer(ID3D11Device* d3dDevice, ID3D11DeviceContext* deviceContext, UINT buffer_type, size_t size_in_bytes, bool dynamic = true, UINT access_flag = D3D11_CPU_ACCESS_WRITE);
		D3D11GraphicsBuffer(ID3D11Device* d3dDevice, ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, bool dynamic = true) : 
			m_device(d3dDevice),
			m_device_context(deviceContext),
			m_buffer(buffer),
			m_dynamic(dynamic)
		{}
		void* Lock(size_t offset, size_t length);
		void Unlock();
		void ReadData(size_t offset, size_t length, void* dest);
		void WriteData(size_t offset, size_t length, const void* source, bool discardBuffer = false, bool mapped = true);
		void update(const void* source);
		ID3D11Buffer* get_buffer() { return m_buffer; };		
	};
}
#endif // !D3D11GRAPHICSBUFFER_H
