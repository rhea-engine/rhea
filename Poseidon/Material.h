#ifndef POSEIDON_MATERIAL_H
#define POSEIDON_MATERIAL_H

#include "Hippios/Renderer.h"
#include "Hippios/GPUProgram.h"
#include "Hippios/GraphicsBuffer.h"
#include <Components/Math/Vector4.h>
#include "Shader.h"

namespace Poseidon
{
	class Core;
	class Shader;
	class Texture;

	class Material
	{
	public:		
		Material::Material(Core* grahpics_core);
		~Material();
		
		// set the shader
		void setShader(Shader* shader);

		// get the shader
		Shader* getShader() { return m_shader; }

		// set the draw topology
		void setTopology(int topology) { m_topology = topology; }	

		// set the current material as active material
		void activate(int mat_buf_register);

		// handle textures		
		
		// add a texture
		void addTexture(Texture* texture); 

		// return the list of textures
		std::vector<Texture*>& getTextures() { return m_textures; }

		Hippios::GPUProgram* getVShader();
		Hippios::GPUProgram* getPShader();	
		
		void SetColor(int slot, const Math::Vector4& color);
		void SetFloat(int slot, float value);
		void SetInt(int slot, int value);

		void* getMaterialBuffer() { return &per_mat_buf; }

	protected:
		Core* m_grahpics_core;
		Shader* m_shader;
		int m_topology;
		std::vector<Texture*> m_textures;
		float* m_mat_buffer;
		size_t m_mat_buffer_size;
		std::vector<Shader::MatBufferLayoutElement>* m_layout;
		struct MatBuffer
		{
			Math::Vector4 ambient;
			Math::Vector4 diffuse;
			Math::Vector4 specular;
			Math::Vector4 emissive;
			float shininess;
			float has_normal_map;
			int num_textures;
			float padding[1];
		} per_mat_buf;
		Hippios::GraphicsBuffer* m_material_buffer;
	};
}

#endif // !POSEIDON_MATERIAL_H
