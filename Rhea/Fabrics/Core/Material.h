#ifndef RHEA_MATERIAL
#define RHEA_MATERIAL

#include "Rhea/Resource.h"

namespace Rhea
{
	class Application;
	class ImageResource;
	class TextResource;

	class Material : public Resource
	{
	public:
		Material(std::string name) :
			m_app(nullptr),
			m_shader(nullptr),
			Resource(name, "//meworla/mesh")
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

	protected:
		Application* m_app;
		TextResource* m_shader;
		std::vector<ImageResource*> m_textures;		
	};
}
		
#endif // !RHEA_MATERIAL
