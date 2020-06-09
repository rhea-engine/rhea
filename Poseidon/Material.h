#ifndef POSEIDON_MATERIAL_H
#define POSEIDON_MATERIAL_H

#include "Hippios/Renderer.h"
#include "Hippios/GPUProgram.h"
#include "Hippios/GraphicsBuffer.h"

namespace Poseidon
{
	class Shader;
	class Texture;

	class Material
	{
	public:		
		Material::Material(Hippios::Renderer* renderer);
		~Material();
		
		// set the shader
		void setShader(Shader* shader) { m_shader = shader; }

		// get the shader
		Shader* getShader() { return m_shader; }

		// set the draw topology
		void setTopology(int topology) { m_topology = topology; }

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
		Hippios::Renderer* m_renderer;
		Shader* m_shader;
		int m_topology;
		std::vector<Texture*> m_textures;
	};
}

#endif // !POSEIDON_MATERIAL_H
