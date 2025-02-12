#ifndef RHEA_MATERIAL
#define RHEA_MATERIAL

#include "Rhea/Unit.h"
#include <Components/Math/Vector3.h>
#include <Math/Vector4.h>

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

		// set texture to specific slot
		void setTexture(int slot, ImageResource* res) { if (slot >= m_textures.size() && res) m_textures.resize(slot + 1); m_textures[slot] = res; }

		// set shader resource
		void setShader(TextResource* shader) { m_shader = shader; }

		// get shader resource
		TextResource* getShader() { return m_shader; }

		// set shader resource
		void setShaderRes(Shader* shader);

		// get shader resource
		Shader* getShaderRes() { return m_shader_res; }		

		// set / get material properties

		// set color for id
		void setColor(int propertyID, Math::Vector3 color) { if (propertyID < m_colors.size()) m_colors[propertyID] = color; }
		void setColor(int propertyID, Math::Vector4 color) { if (propertyID < m_colors.size()) m_colors[propertyID] = color; }

		// get color for id
		Math::Vector4 getColor(int propertyID) { if (propertyID < m_colors.size()) return m_colors[propertyID]; else return Math::Vector4(0, 0, 0, 0); }

		// set a float value by id
		void setFloat(int propertyID, float value) { if (propertyID < m_floats.size()) m_floats[propertyID] = value; }
		// get a float value by id
		float getFloat(int propertyID) { if (propertyID < m_floats.size()) return m_floats[propertyID]; else return 0.0f; }

		// set an int value by id
		void setInt(int propertyID, int value) { if (propertyID < m_ints.size()) m_ints[propertyID] = value; }
		// get an int value by id
		int getInt(int propertyID) { if (propertyID < m_ints.size()) return m_ints[propertyID]; else return 0; }

	protected:
		TextResource* m_shader;
		Shader* m_shader_res;
		std::vector<ImageResource*> m_textures;
		std::vector<Math::Vector4> m_colors;
		std::vector<float> m_floats;
		std::vector<int> m_ints;
	};
}
		
#endif // !RHEA_MATERIAL
