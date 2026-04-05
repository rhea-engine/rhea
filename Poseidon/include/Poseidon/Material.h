#ifndef POSEIDON_MATERIAL_H
#define POSEIDON_MATERIAL_H

#include "Hippios/Renderer.h"
#include "Hippios/ShaderProgram.h"
#include "Hippios/GraphicsBuffer.h"
#include <Math/Vector4.h>
#include "Shader.h"
#include <Math/Vector2.h>

namespace poseidon
{
	class Core;
	class Shader;
	class Texture;

	class Material
	{
	public:		
		Material(Core* grahpics_core);
		~Material();
		
		// set the shader
		void setShader(Shader* shader);

		// get the shader
		Shader* getShader() { return m_shader; }

		// set the draw topology
		void setTopology(int topology) { m_topology = topology; }	

		// set the current material as active material
		void activate(int mat_buf_register, int mat_buf_register_vs = 0);

		// handle textures		
		
		// add a texture
		void addTexture(Texture* texture); 

		// return the list of textures
		std::vector<Texture*>& getTextures() { return m_textures; }

		hippios::ShaderProgram* getShaderProgram();		
		
		void SetColor(int slot, const math::Vector4& color);
		void SetFloat(int slot, float value);
		void SetInt(int slot, int value);

		void SetFloatVS(int slot, float value); 
		void SetFloat2VS(int slot, const math::Vector2& value);		
		void SetFloat4VS(int slot, const math::Vector4& value);
		void SetIntVS(int slot, int value);
		
		void* getMaterialBuffer() { return &per_mat_buf; }
		void setMaterialVSBuffer(const void* buf);

	protected:
		Core* m_grahpics_core;
		Shader* m_shader;
		int m_topology;
		std::vector<Texture*> m_textures;
		char* m_mat_buffer;
		size_t m_mat_buffer_size;
		std::vector<Shader::MatBufferLayoutElement>* m_layout;
		struct MatBuffer
		{
			math::Vector4 ambient;
			math::Vector4 diffuse;
			math::Vector4 specular;
			math::Vector4 emissive;
			float shininess;
			float has_normal_map;
			int num_textures;
			float padding[1];
		} per_mat_buf;
		hippios::GraphicsBuffer* m_material_buffer;
		// VS Buffer
		char* m_mat_buffer_vs;
		size_t m_mat_buffer_size_vs;
		std::vector<Shader::MatBufferLayoutElement>* m_layout_vs;
		hippios::GraphicsBuffer* m_material_buffer_vs;
		bool m_mat_buf_vs_dirty;
	};
}

#endif // !POSEIDON_MATERIAL_H
