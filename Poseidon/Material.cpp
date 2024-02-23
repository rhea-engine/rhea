#include "Material.h"
#include "Shader.h"
#include <cassert>
#include <Core.h>

namespace Poseidon
{
	Material::Material(Core* graphics_core) :
		m_grahpics_core(graphics_core),
		m_shader(nullptr),
		m_topology(4),
		m_mat_buffer(nullptr),
		m_mat_buffer_size(0),
		m_layout(nullptr),
		m_material_buffer(nullptr)
	{
		per_mat_buf = {};
	}

	Material::~Material()
	{
		if (m_shader)
			delete m_shader;
		if (m_mat_buffer)
			free(m_mat_buffer);
	}

	void Material::activate(int mat_buf_register)
	{		
		// activate the shader (the pso)
		m_shader->activate();

		// set the material buffer
		if (m_material_buffer && mat_buf_register > 0) {
			// bind the material buffer
			m_material_buffer->update((void*)&per_mat_buf);
			m_grahpics_core->get_renderer()->bindGPUBuffer(m_material_buffer, mat_buf_register, "PS");
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
	}

	void Material::addTexture(Texture* texture)
	{
		m_textures.push_back(texture);
		if (m_textures.size() > 1)
			per_mat_buf.has_normal_map = 1.0f;
	}

	Hippios::GPUProgram* Material::getVShader()
	{
		if (!m_shader)
			return nullptr;
		return m_shader->getVShader();
	}

	Hippios::GPUProgram* Material::getPShader()
	{
		if (!m_shader)
			return nullptr;
		return m_shader->getPShader();
	}

	void Material::SetColor(int slot, const Math::Vector4& color)
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

		//if (m_mat_buffer && (*m_layout).size() > slot && (*m_layout)[slot].offset + sizeof(Math::Vector4) < m_mat_buffer_size)
		//	memcpy(m_mat_buffer + (*m_layout)[slot].offset, &color, sizeof(Math::Vector4));
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
}