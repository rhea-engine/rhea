#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <stdio.h>

#include "D3D11GraphicsBuffer.h"
#include <cassert>

namespace Hippios
{
	D3D11GraphicsBuffer::D3D11GraphicsBuffer(ID3D11Device* d3dDevice, ID3D11DeviceContext* deviceContext, UINT buffer_type, size_t size_in_bytes, bool dynamic, UINT access_flag)
	{
		m_device = d3dDevice;
		m_device_context = deviceContext;		
		m_size_in_bytes = size_in_bytes;
		m_dynamic = dynamic;
		
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		if (dynamic) {
			bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
			bd.CPUAccessFlags = access_flag;    // allow CPU to write in buffer?
		}
		else {
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0;
		}
		bd.ByteWidth = (UINT)m_size_in_bytes;            
		bd.BindFlags = buffer_type;					   // set buffer type
		

		HRESULT hr = m_device->CreateBuffer(&bd, NULL, &m_buffer);       // create the buffer		
		if (FAILED(hr))
		{
			m_buffer = nullptr;
			printf("Could not create graphics buffer - expect failure\n");
			assert(m_buffer != nullptr);
		}
	}

	D3D11GraphicsBuffer::~D3D11GraphicsBuffer()
	{
		if (m_buffer) m_buffer->Release();
	}

	void* D3D11GraphicsBuffer::Lock(size_t offset, size_t length)
	{
		if (m_is_locked) printf("Buffer already locked!");
		m_is_locked = true;

		void* pRet = NULL;
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		mappedSubResource.pData = NULL;
		//D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
		//HRESULT hr = mDeviceContext->Map(mVBuffer, 0, mapType, 0, &mappedSubResource);
		HRESULT hr = m_device_context->Map(m_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubResource);    // map the buffer
		if (FAILED(hr))
		{
			return 0;
		}

		pRet = static_cast<void*>(static_cast<char*>(mappedSubResource.pData) + offset);

		return pRet;
	};

	void D3D11GraphicsBuffer::Unlock()
	{
		if (!m_is_locked) printf("Buffer is not locked!\n");
		m_is_locked = false;
		m_device_context->Unmap(m_buffer, NULL);
	};

	void D3D11GraphicsBuffer::ReadData(size_t offset, size_t length, void* dest)
	{
		void* source = this->Lock(offset, length);
		memcpy(dest, source, length);
		this->Unlock();
	}

	void D3D11GraphicsBuffer::WriteData(size_t offset, size_t length, const void* source, bool discardBuffer, bool mapped)
	{
		if (mapped && m_dynamic)
		{
			void* dest = this->Lock(offset, length);
			memcpy(dest, source, length);
			this->Unlock();
		}
		else if (m_buffer) {
			m_device_context->UpdateSubresource(m_buffer, 0, NULL, source, 0, 0);
//			m_device_context->VSSetConstantBuffers(0, 1, &m_buffer);
		}
	}
	
	void D3D11GraphicsBuffer::update(const void* source)
	{
		m_device_context->UpdateSubresource(m_buffer, 0, NULL, source, 0, 0);
	}
}