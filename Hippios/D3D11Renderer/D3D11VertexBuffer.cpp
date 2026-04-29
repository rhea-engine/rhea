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

#include "D3D11VertexBuffer.h"

namespace hippios
{
	D3D11VertexBuffer::D3D11VertexBuffer(ID3D11Device* d3d_device, ID3D11DeviceContext* device_context, size_t vertex_size, size_t num_vertices, bool dynamic) :
		is_dirty(true)
	{
		m_device = d3d_device;
		m_device_context = device_context;
		// compute size
		m_size_in_bytes = vertex_size * num_vertices;
		m_vertex_size = vertex_size;
		m_num_vertices = num_vertices;

		m_buffer = std::make_unique<D3D11GraphicsBuffer>(m_device, m_device_context, D3D11_BIND_VERTEX_BUFFER, m_size_in_bytes, dynamic);
	}

	void* D3D11VertexBuffer::Lock(size_t offset, size_t length)
	{
		return m_buffer->Lock(offset, length);
	};
	void D3D11VertexBuffer::Unlock()
	{
		m_buffer->Unlock();
	};
	void D3D11VertexBuffer::ReadData(size_t offset, size_t length, void* dest)
	{
		m_buffer->ReadData(offset, length, dest);
	}

	void D3D11VertexBuffer::WriteData(size_t offset, size_t length, const void* source, bool discard_buffer, bool mapped)
	{
		m_buffer->WriteData(offset, length, source, discard_buffer, mapped);
		is_dirty = true;
	}

	ID3D11Buffer* D3D11VertexBuffer::get_buffer()
	{
		return m_buffer->get_buffer();
	}
}