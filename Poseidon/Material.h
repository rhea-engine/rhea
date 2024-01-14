#ifndef POSEIDON_MATERIAL_H
#define POSEIDON_MATERIAL_H

#include "Hippios/Renderer.h"
#include "Hippios/GPUProgram.h"
#include "Hippios/GraphicsBuffer.h"

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
		void setShader(Shader* shader) { m_shader = shader; }

		// get the shader
		Shader* getShader() { return m_shader; }

		// set the draw topology
		void setTopology(int topology) { m_topology = topology; }

		// set alpha blending
		void setAlphaBlending(bool enable) { m_alpha_blending = enable; }

		// set stencil testing to stencil test with given name
		void setStencilTest(std::string test_name) { m_stencil_test = test_name; }

		void finalize();

		// set the current material as active material
		void activate();

		// handle textures		
		
		// add a texture
		void addTexture(Texture* texture); 

		// return the list of textures
		std::vector<Texture*>& getTextures() { return m_textures; }

		Hippios::GPUProgram* getVShader();
		Hippios::GPUProgram* getPShader();
	protected:
		Core* m_grahpics_core;
		Shader* m_shader;
		int m_topology;
		std::vector<Texture*> m_textures;
		bool m_alpha_blending;
		Hippios::PipelineStateObject* m_pso;
		std::string m_stencil_test;
	};
}

#endif // !POSEIDON_MATERIAL_H
