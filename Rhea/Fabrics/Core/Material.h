#ifndef RHEA_MATERIAL
#define RHEA_MATERIAL

#include "Rhea/Unit.h"

namespace Rhea
{
	class Application;
	class ImageResource;
	class TextResource;

	class Material : public Unit
	{
	public:
		Material(std::string name) :
			Material(name, "//meworla/material")
		{}
		Material(std::string name, std::string type_name) :
			m_app(nullptr),
			m_shader(nullptr),
			Unit(name, type_name)
		{}

		// get textures
		std::vector<ImageResource*>& getTextures() { return m_textures; }

		// add a texture
		void addTexture(ImageResource* res) { m_textures.push_back(res); }

		// set shader resource
		void setShader(TextResource* shader) { m_shader = shader; }

		// get shader resource
		TextResource* getShader() { return m_shader; }

		// alpha blending
		bool getAlphaBlending();

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Application* m_app;
		TextResource* m_shader;
		std::vector<ImageResource*> m_textures;		
	};
}
		
#endif // !RHEA_MATERIAL
