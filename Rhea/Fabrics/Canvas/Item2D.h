#ifndef RHEA_ITEM2D_H
#define RHEA_ITEM2D_H

#include "../Core/Material.h"
#include "../Core/Transform.h"
#include "../Core/Item.h"
#include <Math/Vector2.h>

using Math::Vector2;

namespace Rhea
{
	class Item2D : public Item
	{
		MECLASS(Item2D, Item)
	public:
		Item2D(std::string name) :
			Item2D(name, "//meworla/Item2D")
		{}
		Item2D(std::string name, std::string type_name) :
			m_size(0.0f, 0.0f),
			m_position(0.0f, 0.0f),
			m_bg_image(nullptr),
			Item(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::Export> getReferences();

		// ready event handler
		void ready();

		virtual void setMesh();

		void move(float _x, float _y) { m_position.x = _x; m_position.y = _y; }
		void resize(float _w, float _h) { m_size.x = _w; m_size.y = _h; }
		void update();

		void setBackgroundColor(int r, int g, int b, float opacity = 1.0f);
		void setBackgroundImage(ImageResource* tex);

		// get dimensions
		float X() { return m_position.x; }
		float Y() { return m_position.y; }
		float W() { return m_size.x; }
		float H() { return m_size.y; }

	protected:
		// dimensions in screen space
		
		// current position in pixel
		Vector2 m_position;
		// current  width in pixel
		Vector2 m_size;

		// background-color
		Math::Vector4 m_bg_color;

		// background image
		ImageResource* m_bg_image;

		void CreateMaterial();
		void UpdateMaterial();
		std::unique_ptr<Material> m_material;
	};
}

#endif // !RHEA_ITEM2D_H
