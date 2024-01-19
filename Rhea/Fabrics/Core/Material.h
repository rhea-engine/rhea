#ifndef RHEA_MATERIAL
#define RHEA_MATERIAL

#include "Rhea/Unit.h"

namespace Rhea
{
	class Application;
	class ImageResource;
	class TextResource;
	class Shader;

	class Material : public Unit
	{
		MECLASS(Material, Unit)
	public:
		Material(std::string name) :
			Material(name, "//meworla/material")
		{}
		Material(std::string name, std::string type_name) :
			m_shader(nullptr),
			m_shader_res(nullptr),
			Unit(name, type_name)
		{}
		std::vector<Unit::Export> getReferences();

		void ready() override;

		// get textures
		std::vector<ImageResource*>& getTextures() { return m_textures; }

		// add a texture
		void addTexture(ImageResource* res) { m_textures.push_back(res); }

		// set shader resource
		void setShader(TextResource* shader) { m_shader = shader; }

		// get shader resource
		TextResource* getShader() { return m_shader; }

		// set shader resource
		void setShaderRes(Shader* shader) { m_shader_res = shader; }

		// get shader resource
		Shader* getShaderRes() { return m_shader_res; }		

	protected:
		TextResource* m_shader;
		Shader* m_shader_res;
		std::vector<ImageResource*> m_textures;	
	};
}
		
#endif // !RHEA_MATERIAL
