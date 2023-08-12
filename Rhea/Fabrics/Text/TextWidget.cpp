#include "TextWidget.h"
#include "Application.h"
#include "Fabrics/Media/ImageResource.h"
#include "Resources/TextResource.h"
#include "UnitTree.h"
#include "Resources/ResourceList.h"
#include <Psio/Log.h>
#include <Graphics/GraphicsService.h>
#include <GUI/Widget.h>

namespace Rhea
{
	void TextWidget::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item2D::setup(app, attributes, values);
		
		m_text = values["text"];
		m_max_length = m_text.length();
		m_word_wrap = getValue("word_wrap", true);
		m_multiline = getValue("multiline", true);
	}

	std::vector<Unit::Export> TextWidget::getReferences()
	{
		std::vector<Export> refs = Item2D::getReferences();
		if (m_values.find("font") != m_values.end()) {
			refs.push_back({ "font", "//meworla/bitmapfont", m_values["font"] });
		}		
		return refs;
	}

	void TextWidget::ready()
	{
		Item2D::ready();

		// load and add font and text
		if (m_values.find("font") != m_values.end())
		{
			m_font = doCast<Font>(m_app->getUnitTree()->find(m_values["font"], m_attributes["basepath"], false));
			if (!m_font)
				LOG(LOG_ERROR) << "TextWidget: Font '" << (std::string)m_values["font"] << "' not found";
		}
		else {
			if (doCast<Widget>(m_parent)) {
				std::vector<Variant>* font_prop = ((Widget*)m_parent)->getStyle("font");
				if (font_prop) {
					m_font = doCast<Font>(m_app->getUnitTree()->find((*font_prop)[0]));
					if (!m_font)
						LOG(LOG_ERROR) << "font " << (std::string)(*font_prop)[0] << " not found";
				}
				else {
					DLOG(LOG_DEBUG) << "font not set in parents";
				}
			}
		}

		if (m_font) {
			m_font->ready();
			m_material = m_font->getMaterial();

			// add the mesh
			if (!m_mesh) {

				std::unordered_map<std::string, std::string> attributes;
				std::unordered_map<std::string, Variant> values;
				m_mesh = static_cast<Mesh*>(m_app->getUnitTree()->createUnit(m_pathname + "_mesh", "//meworla/mesh", attributes, values));
				m_mesh->setMaterial(m_material);
				// text is dynamic
				m_mesh->setDynamic(true);

				setMesh();

				m_render_component->setMesh(m_mesh);
			}

		}
	}
	
	void TextWidget::setMesh()
	{
		Rhea::Font* font = getFont();
		std::string sentence = getText();

		float t_x = (float)X() + m_offset_x;
		float t_y = (float)Y();
		float t_w = (float)W() - m_offset_x;
		float t_h = (float)H();

		// adjust to height of viewport (y is negative)
		GraphicsService* gsrv = (GraphicsService*)m_app->getService("graphics");
		t_y = gsrv->getViewportHeight() - t_y;

		float z = 0.0f;
		float draw_x = t_x;
		float draw_y = t_y;
		Glyph* glyph;
		float f_sz = font->getFontSize();

		std::vector<Mesh::vertex> vertices;
		std::vector<unsigned long> v_indices;

		int v_c = 0; // vertex count
		for (int i = 0; i < sentence.size(); i++) {

			unsigned char c = static_cast<unsigned char>(sentence[i]);

			if (m_multiline && (c == '\n')) {
				// go to next line
				if (draw_y - (2 * f_sz) < t_y - t_h) { // NOTE: "-" because coords are flipped
					// no - stop rendering
					break;
				}
				// go to next line
				draw_x = t_x;
				draw_y -= f_sz;
				continue;
			}
			glyph = font->getGlyph((unsigned int)c);

			// safety check
			if (!glyph)
				return;

			// top/left=0/0
			float f_left = glyph->left;
			float f_right = glyph->right;
			float f_top = glyph->top;
			float f_bottom = glyph->bottom;

			// do we have space for the next char?
			if (draw_x + glyph->advance > t_w + t_x) {

				// check if word-wrapping is on
				// if not we break out
				if (!m_word_wrap)
					break;

				// do we have space for the next line?
				if (draw_y - (2 * f_sz) < t_y - t_h) { // NOTE: "-" because coords are flipped
					// no - stop rendering
					break;
				}
				// go to next line
				draw_x = t_x;
				draw_y -= f_sz;
			}

			// check clipping
			if (draw_x >= X()) {

				// first triangle
				// top-left
				vertices.push_back({ draw_x, draw_y, z, f_left, f_top });
				v_indices.push_back(v_c * 4);
				// bottom-right
				vertices.push_back({ draw_x + glyph->advance, draw_y - f_sz, z, f_right, f_bottom });
				v_indices.push_back(v_c * 4 + 1);
				// bottom-left
				vertices.push_back({ draw_x, draw_y - f_sz, z, f_left, f_bottom });
				v_indices.push_back(v_c * 4 + 2);

				// second triangle
				// top-left
				v_indices.push_back(v_c * 4 + 0);
				// top-right
				vertices.push_back({ draw_x + glyph->advance, draw_y, z, f_right, f_top });
				v_indices.push_back(v_c * 4 + 3);
				// bottom-right
				v_indices.push_back(v_c * 4 + 1);

				v_c++;
			}

			// advance by glyph width
			draw_x += glyph->advance;
		}

		if (!v_indices.size()) {
			m_render_component->hide();
			return;
		} else {
			m_render_component->show();
		}


		// fill up the buffer with zeros
		size_t v_len = vertices.size();
		if (m_vertex_size > v_len) {
			for (size_t j = v_len; j < m_vertex_size; j++) {
				vertices.push_back({ 0,0,0,0,0 });
			}	
		}
		else {
			m_vertex_size = v_len;
		}

		m_mesh->setBuffers(vertices, v_indices);
		m_mesh->sendEvent("update", nullptr);
	}

	Vector2 TextWidget::getCursorPosition(size_t pos)
	{
		Vector2 res;
		Rhea::Font* font = getFont();
		std::string sentence = getText();
		float f_sz = font->getFontSize();
		Glyph* glyph;

		if (pos > sentence.length())
			pos = sentence.length();

		res.x = X() + m_offset_x;
		res.y = Y();
		float t_w = res.x + (float)W() - m_offset_x;

		for (int i = 0; i < pos; i++) {

			unsigned char c = static_cast<unsigned char>(sentence[i]);

			if (m_multiline && (c == '\n')) {
				// go to next line
				if (res.y + (2 * f_sz) > Y() + H()) { 
					// no - stop rendering
					break;
				}
				// go to next line
				res.x = X();
				res.y += f_sz;
				continue;
			}
			glyph = font->getGlyph(c);
			
			// safety check
			if (!glyph)
				continue;

			// advance by glyph width
			if (1 || res.x + glyph->advance < t_w) {
				res.x += glyph->advance;
			}
			else {
				// TODO: word-wrapping?

				break;
			}
		}

		return res;
	}

	size_t TextWidget::getTextPosition(Vector2 pos)
	{
		size_t txt_pos = 0;
		Rhea::Font* font = getFont();
		std::string sentence = getText();
		float f_sz = font->getFontSize();
		Glyph* glyph;
		float x = X() + m_offset_x;

		for (int i = 0; i < sentence.size(); i++) {
			glyph = font->getGlyph(static_cast<unsigned char>(sentence[i]));
			if (x + glyph->advance > pos.x) {
				break;
			}
			txt_pos++;
			x += glyph->advance;
		}


		return txt_pos;
	}

	float TextWidget::getTextWidth(size_t pos)
	{
		float ret_x = 0.0f;
		Rhea::Font* font = getFont();
		std::string sentence = getText();
		float f_sz = font->getFontSize();
		Glyph* glyph;

		for (int i = 0; i < sentence.size() && i < pos; i++) {
			glyph = font->getGlyph(static_cast<unsigned char>(sentence[i]));
			ret_x += glyph->advance;
		}
		return ret_x;
	}

	void TextWidget::setText(std::string text)
	{
		m_text = text;
		if (m_mesh)
			setMesh();
	}

	void TextWidget::setWordWrap(bool word_wrap) {
		m_word_wrap = word_wrap;
		// TODO: change appearance of text?
	}
}
