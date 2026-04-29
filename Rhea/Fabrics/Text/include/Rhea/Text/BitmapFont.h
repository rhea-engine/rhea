/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_BITMAPFONT_H
#define RHEA_BITMAPFONT_H

#include "Font.h"
#include "Rhea/Core/Material.h"

namespace rhea
{
	class BitmapFont : public Font
	{
		MECLASS(BitmapFont, Font)
	public:
		BitmapFont(std::string name, std::string type_name) :
			m_dim_w(256),
			m_dim_h(256),
			m_cell_w(0),
			m_cell_h(0),
			m_start_letter(32),
			Font(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::Export> getReferences();

		// ready handler
		void ready();

		// get info about a glyph
		Glyph* getGlyph(unsigned int letter);

	protected:
		int m_dim_w;
		int m_dim_h;
		float m_cell_w;
		float m_cell_h;
		unsigned int m_start_letter;
		std::vector<Glyph> m_glyphs;
	};
}

#endif //!RHEA_BITMAPFONT_H
