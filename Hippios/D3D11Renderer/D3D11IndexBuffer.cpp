/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <stdio.h>

#include "D3D11IndexBuffer.h"

namespace hippios
{
	D3D11IndexBuffer::D3D11IndexBuffer(ID3D11Device* d3dDevice, ID3D11DeviceContext* device_context, size_t index_stride, size_t num_faces, bool dynamic) :
		is_dirty(true)
	{
		m_device = d3dDevice;
		m_device_context = device_context;
		// compute size
		m_size_in_bytes = index_stride * num_faces * 3;
		m_index_stride = index_stride;
		m_num_faces = num_faces;

		if (index_stride == 2)
			m_index_format = DXGI_FORMAT_R16_UINT;
		else
			m_index_format = DXGI_FORMAT_R32_UINT;

		m_buffer = std::make_unique<D3D11GraphicsBuffer>(m_device, m_device_context, D3D11_BIND_INDEX_BUFFER, m_size_in_bytes, dynamic);
	}

	void* D3D11IndexBuffer::Lock(size_t offset, size_t length)
	{
		return m_buffer->Lock(offset, length);
	};
	void D3D11IndexBuffer::Unlock()
	{
		m_buffer->Unlock();
	};
	void D3D11IndexBuffer::ReadData(size_t offset, size_t length, void* dest)
	{
		m_buffer->ReadData(offset, length, dest);
	}

	void D3D11IndexBuffer::WriteData(size_t offset, size_t length, const void* source, bool discardBuffer, bool mapped)
	{
		m_buffer->WriteData(offset, length, source, discardBuffer, mapped);
		is_dirty = true;
	}

	ID3D11Buffer* D3D11IndexBuffer::get_buffer()
	{
		return m_buffer->get_buffer();
	}
}