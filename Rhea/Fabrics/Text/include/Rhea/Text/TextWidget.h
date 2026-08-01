/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_TEXTWIDGET_H
#define RHEA_TEXTWIDGET_H

#include <Rhea/Resources/Resource.h>
#include <Rhea/Core/Material.h>
#include "Font.h"
#include <Rhea/Core/Item2D.h>
#include "Math/Vector2.h"

namespace rhea
{
	class FontFamily;

	class TextWidget : public Item2D
	{
		RECLASS(TextWidget, Item2D, "//meworla/textwidget")
	public:		
		TextWidget(std::string name, std::string type_name) :
			m_font(nullptr),
			m_font_family(nullptr),
			Item2D(name, type_name),
			m_requested_font_size(0.0f),
			m_max_length(0),
			m_vertex_size(0),
			m_word_wrap(true),
			m_multiline(true),
			m_offset_x(0.0f),
			m_text_dirty(true)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		std::vector<Unit::ResourceReference> getReferences();

		// ready event handler
		void ready();

		void setFont(Font* font);
		void setFontFamily(FontFamily* family);
		void setFontSize(float font_size);

		void syncRender() override;

		// reset the mesh for the text
		void computeMesh() override;

		// get the Text
		std::string getText() { return m_text; }

		// set a new text
		void setText(std::string text);

		// get the max length of text
		size_t getMaxLength() { return m_max_length; }
		// set the max length of text
		void setMaxLength(int max_length) { m_max_length = max_length; }

		// get relative pixel position of position
		Vector2 getCursorPosition(size_t pos);
		// get text position of pixel (rounded up)
		size_t getTextPosition(Vector2 pos);
		// get length of text (only for one-liner useful)
		float getTextWidth(size_t pos);
		// offset to one side for text input
		void setOffset(float offset) { m_offset_x = offset; }

		// get size of current text
		Vector2 getTextSize(Vector2 size_constraints);

		// set word wrap option
		void setWordWrap(bool word_warp);

		// get the font
		Font* getFont() { return m_font; }
		FontFamily* getFontFamily() { return m_font_family; }
		float getRequestedFontSize() const { return m_requested_font_size; }

		// mark text dirty
		void markTextDirty() { m_text_dirty = true; invalidateRender(); }

	protected:
		std::string m_text;
		Font* m_font;
		FontFamily* m_font_family;
		float m_requested_font_size;
		size_t m_max_length;
		size_t m_vertex_size;
		bool m_word_wrap;
		bool m_multiline;
		float m_offset_x;

		bool m_text_dirty;
	};
}

#endif // !RHEA_TEXTWIDGET_H
