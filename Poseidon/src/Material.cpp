/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include <cassert>
#include <Poseidon/Core.h>

namespace poseidon
{
	Material::Material(Core* graphics_core) :
		m_grahpics_core(graphics_core),
		m_shader(nullptr),
		m_topology(4),
		m_mat_buffer(nullptr),
		m_mat_buffer_size(0),
		m_layout(nullptr),
		m_material_buffer(nullptr),
		m_mat_buffer_vs(nullptr),
		m_mat_buffer_size_vs(0),
		m_layout_vs(nullptr),
		m_material_buffer_vs(nullptr),
		m_mat_buf_vs_dirty(false)
	{
		per_mat_buf = {};
	}

	Material::~Material()
	{
		if (m_mat_buffer)
			free(m_mat_buffer);
		if (m_mat_buffer_vs)
			free(m_mat_buffer_vs);
	}

	void Material::activate(int mat_buf_register, int mat_buf_register_vs)
	{		
		// activate the shader (the pso)
		m_shader->activate();

		// set the material buffer
		if (m_material_buffer && mat_buf_register > 0) {
			// bind the material buffer
			m_material_buffer->update((void*)&per_mat_buf);
			m_grahpics_core->get_renderer()->bindGPUBuffer(m_material_buffer, mat_buf_register, hippios::ShaderStage::Fragment);
		}

		if (m_material_buffer_vs && mat_buf_register_vs > 0 && m_mat_buffer_vs) {
			if (m_mat_buf_vs_dirty)
				m_material_buffer_vs->update(m_mat_buffer_vs);
			m_grahpics_core->get_renderer()->bindGPUBuffer(m_material_buffer_vs, mat_buf_register_vs, hippios::ShaderStage::Vertex);
		}
	}

	void Material::setShader(Shader* shader)
	{
		if (m_mat_buffer) {
			free(m_mat_buffer);
			m_mat_buffer = nullptr;
		}

		m_shader = shader;
		
		// setup the material buffer
		m_layout = m_shader->getMatBufLayout();
		// size of the material buffer
		m_mat_buffer_size = m_shader->getMatBufSize();
		// Calculate the remainder when dividing by 16
		int remainder = m_mat_buffer_size % 16;
		// Calculate the padding needed to reach the next multiple of 16
		int padding = (remainder == 0) ? 0 : (16 - remainder);
		// Add the padding to the original size
		m_mat_buffer_size += padding;

		if (m_mat_buffer_size) {

			// setup the material buffer			
			m_material_buffer = m_grahpics_core->getGraphicsBuffer("material_buffer", m_mat_buffer_size);

		//	m_mat_buffer = (float*)malloc(m_mat_buffer_size);
		//	if (m_mat_buffer) memset(m_mat_buffer, 0, m_mat_buffer_size);
		}

		// VS
		m_mat_buffer_size_vs = m_shader->getMatBufSizeVS();
		if (m_mat_buffer_size_vs > 0) {
			// setup the material buffer
			m_layout_vs = m_shader->getMatBufLayoutVS();

			// Calculate the remainder when dividing by 16
			remainder = m_mat_buffer_size_vs % 16;
			// Calculate the padding needed to reach the next multiple of 16
			padding = (remainder == 0) ? 0 : (16 - remainder);
			// Add the padding to the original size
			m_mat_buffer_size_vs += padding;

			if (m_mat_buffer_vs) {
				free(m_mat_buffer_vs);  // Free if already allocated
			}
			m_mat_buffer_vs = (char*)malloc(shader->getMatBufSize());
			if (m_mat_buffer_vs) memset(m_mat_buffer_vs, 0, shader->getMatBufSize());
			
			// setup the material buffer			
			m_material_buffer_vs = m_grahpics_core->getGraphicsBuffer("material_buffer_vs", m_mat_buffer_size_vs);

			m_mat_buf_vs_dirty = true;
		}
	}

	void Material::addTexture(Texture* texture)
	{
		m_textures.push_back(texture);
		if (m_textures.size() > 1)
			per_mat_buf.has_normal_map = 1.0f;
		per_mat_buf.num_textures = (int)m_textures.size();
	}

	hippios::ShaderProgram* Material::getShaderProgram()
	{
		if (!m_shader)
			return nullptr;
		return m_shader->getShaderProgram();
	}

	void Material::SetColor(int slot, const math::Vector4& color)
	{
		switch (slot) {
		case 0:
			per_mat_buf.ambient = color;
			break;
		case 1:
			per_mat_buf.diffuse = color;
			break;
		case 2:
			per_mat_buf.specular = color;
			break;
		case 3:
			per_mat_buf.emissive = color;
			break;
		}

		//if (m_mat_buffer && (*m_layout).size() > slot && (*m_layout)[slot].offset + sizeof(math::Vector4) < m_mat_buffer_size)
		//	memcpy(m_mat_buffer + (*m_layout)[slot].offset, &color, sizeof(math::Vector4));
	}
	
	void Material::SetFloat(int slot, float value)
	{
		switch (slot) {
		case 4:
			per_mat_buf.shininess = value;
		}

	//	if (m_mat_buffer)
	//		memcpy(m_mat_buffer + (*m_layout)[slot].offset, &value, sizeof(float));
	}

	void Material::SetInt(int slot, int value)
	{
	//	if (m_mat_buffer)
	//		memcpy(m_mat_buffer + (*m_layout)[slot].offset, &value, sizeof(int));
	}
	
	void Material::SetFloat2VS(int slot, const math::Vector2& value)
	{
		if (m_mat_buffer_vs) {
			memcpy((char*)m_mat_buffer_vs + (*m_layout_vs)[slot].offset, &value, sizeof(math::Vector2));		
			m_mat_buf_vs_dirty = true;
		}
	}

	void Material::SetFloat4VS(int slot, const math::Vector4& value)
	{
		if (m_mat_buffer_vs) {
			memcpy((char*)m_mat_buffer_vs + (*m_layout_vs)[slot].offset, &value, sizeof(math::Vector4));
			m_mat_buf_vs_dirty = true;
		}
	}

	void Material::SetFloatVS(int slot, float value)
	{
		if (m_mat_buffer) {
			memcpy(m_mat_buffer_vs + (*m_layout_vs)[slot].offset, &value, sizeof(float));
			m_mat_buf_vs_dirty = true;
		}
	}

	void Material::SetIntVS(int slot, int value)
	{
		if (m_mat_buffer) {
			memcpy(m_mat_buffer_vs + (*m_layout_vs)[slot].offset, &value, sizeof(int));
			m_mat_buf_vs_dirty = true;
		}
	}

	void Material::setMaterialVSBuffer(const void* buf)
	{
		if (m_material_buffer_vs)
			m_material_buffer_vs->update(buf);
	}
}