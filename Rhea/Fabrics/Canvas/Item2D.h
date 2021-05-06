#ifndef RHEA_ITEM2D_H
#define RHEA_ITEM2D_H

#include "Resource.h"
#include "Material.h"

namespace Rhea
{
	class Item2D : public Resource
	{
	public:
		Item2D(std::string name) :
			Item2D(name, "//meworla/Item2D")
		{}
		Item2D(std::string name, std::string type_name) :
			m_material(nullptr),
			x(0),
			y(0),
			w(0),
			h(0),
			Resource(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// get the material
		Material* getMaterial() { return m_material; }

		void move(int _x, int _y) { x = _x; y = _y; }
		void resize(int _w, int _h) { w = _w; h = _h; }

		// get dimensions
		int X() { return x; }
		int Y() { return y; }
		int W() { return w; }
		int H() { return h; }

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Material* m_material;

		// dimensions in screen space
		int x;
		int y;
		int w;
		int h;
	};
}

#endif // !RHEA_ITEM2D_H
