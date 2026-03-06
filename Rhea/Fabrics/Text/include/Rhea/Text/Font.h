#ifndef RHEA_FONT_H
#define RHEA_FONT_H

#include <vector>

#include <Rhea/Unit.h>

namespace Rhea
{
	class Application;
	class ImageResource;
	class Material;

	// definition of a glyph
	// measures are in uv coords
	struct Glyph {
		float left;
		float top;
		float right;
		float bottom;
		int bearing_x;
		int bearing_y;
		float advance;
	};

	class Font : public Unit
	{
		MECLASS_VIRTUAL(Font, Unit)
	public:
		Font(std::string name, std::string type_name) :
			m_material(nullptr),
			m_sz(0.0f),
			Unit(name, type_name)
		{}
		Font(std::string name) :
			Font(name, "//meworla/font")
		{}

		// get material for rendering
		Material* getMaterial() { return m_material; }

		// get info about font
		float getFontSize() { return m_sz; }

		// get info about a glyph
		virtual Glyph* getGlyph(unsigned int letter) = 0;

	protected:
		Material* m_material;
		float m_sz;
	};
}

#endif // !RHEA_FONT_H
