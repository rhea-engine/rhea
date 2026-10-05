/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Text/TextWidget.h"
#include "Rhea/Text/FontFamily.h"
#include "Rhea/Application.h"
#include "Rhea/Resources/ImageResource.h"
#include "Rhea/Resources/TextResource.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Resources/ResourceList.h"
#include <Psio/Log.h>
#include <Rhea/Graphics/IGraphicsService.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Components/MeshComponent.h>

namespace rhea
{
	void TextWidget::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item2D::setup(app, attributes, values);
		
		m_text = values["text"].toString();
		m_requested_font_size = getValue("font_size", 0.0f);
		m_max_length = m_text.length();
		m_word_wrap = getValue("word_wrap", true);
		m_multiline = getValue("multiline", true);
	}

	std::vector<Unit::ResourceReference> TextWidget::getReferences()
	{
		std::vector<ResourceReference> refs = Item2D::getReferences();
		if (m_values.find("font") != m_values.end()) {
			refs.push_back({ "font", "//meworla/bitmapfont", m_values["font"] });
		}
		if (m_values.find("font_family") != m_values.end()) {
			refs.push_back({ "font_family", "//meworla/fontfamily", m_values["font_family"] });
		}
		return refs;
	}

	void TextWidget::ready()
	{
		Item2D::ready();

		VisualComponent* visual_component = getComponent<VisualComponent>();
		if (!visual_component) {
			visual_component = addComponentUnique<VisualComponent>("visual");
		}
		// add to render phase
		visual_component->setRenderPhase(RenderPhase::Overlay);

		// A family takes precedence over the legacy direct font reference.
		if (m_values.find("font_family") != m_values.end())
		{
			FontFamily* family = doCast<FontFamily>(m_app->getUnitTree()->find(
				m_values["font_family"].toString(), m_attributes["basepath"], false));
			if (!family)
				LOG(LOG_ERROR) << "TextWidget: Font family '" << (std::string)m_values["font_family"] << "' not found";
			else
				setFontFamily(family);
		}
		else if (m_values.find("font") != m_values.end())
		{
			Font* font = doCast<Font>(m_app->getUnitTree()->find(m_values["font"].toString(), m_attributes["basepath"], false));
			if (!font)
				LOG(LOG_ERROR) << "TextWidget: Font '" << (std::string)m_values["font"] << "' not found";
			else
				setFont(font);
		}
	}

	void TextWidget::setFontFamily(FontFamily* family)
	{
		m_font_family = family;
		if (!m_font_family)
			return;

		m_font_family->ready();
		Font* resolved_font = m_font_family->resolve(m_requested_font_size);
		if (!resolved_font)
			LOG(LOG_ERROR) << "TextWidget: Font family could not resolve size " << m_requested_font_size;
		else
			setFont(resolved_font);
	}

	void TextWidget::setFontSize(float font_size)
	{
		if (font_size <= 0.0f || m_requested_font_size == font_size)
			return;

		m_requested_font_size = font_size;
		if (m_font_family)
			setFontFamily(m_font_family);
	}

	void TextWidget::setTextColor(math::Color color)
	{
		if (m_text_color == color)
			return;
		m_text_color = color;
		if (!m_text_material || !m_text_material->getShader())
			return;

		m_text_material->setColor(m_text_material->getShader()->getInputSlotId("BaseColor"), m_text_color);
		invalidateRender();
	}

	void TextWidget::setFont(Font* font)
	{
		if (m_font == font)
			return;

		if (font) {
			font->ready();
			Material* font_material = font->getMaterial();
			if (!font_material) {
				LOG(LOG_ERROR) << "TextWidget: Font '" << font->getPathName() << "' has no material";
				return;
			}
			if (!font_material->getShader()) {
				LOG(LOG_ERROR) << "TextWidget: Font '" << font->getPathName() << "' has no shader";
				return;
			}
			if (!m_text_material) {
				// TODO(text): Cache immutable materials by font and color so widgets
				// with the same style can share one. Vertex color is a possible
				// longer-term alternative that would also improve batching.
				m_text_material = new Material(m_pathname + "_text_material");
				m_text_material->setup(m_app, {}, {});
				addChild(m_text_material);
			}
			m_text_material->setValue("shader", font_material->getValue("shader"));
			m_text_material->setShader(font_material->getShader());
			for (size_t slot = 0; slot < font_material->getTextures().size(); ++slot)
				m_text_material->setTexture(static_cast<int>(slot), font_material->getTextures()[slot]);
			m_text_material->setColor(m_text_material->getShader()->getInputSlotId("BaseColor"), m_text_color);
			font_material = m_text_material;

			// make sure visualcomponent is set
			VisualComponent* visual_component = getComponent<VisualComponent>();
			if (!visual_component) {
				visual_component = addComponentUnique<VisualComponent>("visual");
			}
			MeshComponent* mesh_component = getComponent<MeshComponent>();
			if (!mesh_component) {
				mesh_component = addComponentUnique<MeshComponent>("mesh");
			}

			// add the mesh
			if (!mesh_component->getMesh()) {

				std::unordered_map<std::string, std::string> attributes;
				std::unordered_map<std::string, Variant> values;
				Mesh* mesh = m_app->getUnitTree()->createUnit<Mesh>(m_pathname + "_mesh", attributes, values);
				mesh->setMaterial(font_material);
				// text is dynamic
				mesh->setDynamic(true);
				addChild(mesh);
				mesh_component->setMesh(mesh);
			}
			else {
				mesh_component->getMesh()->setMaterial(font_material);
			}
		}

		// Commit the font only after validation so failed assignments can be retried.
		m_font = font;
		markTextDirty();
	}
	
	void TextWidget::syncRender()
	{
		bool changed = false;		

		if (m_text_dirty || m_render_dirty) {
			//LOG(LOG_INFO) << "TextWidget::syncRender() for " << m_pathname << ", is dirty: " << m_text_dirty;

			computeMesh();
			m_text_dirty = false;
			m_render_dirty = false;
			changed = true;
		}

		if (changed) {
			Event e("update", this);
			sendEvent("update", &e);
		}
	}

	void TextWidget::computeMesh()
	{
		rhea::Font* font = getFont();
		std::string sentence = getText();		

		if (!font) {
			LOG(LOG_ERROR) << "TextWidget has no font defined";
			return;
		}

		float t_x = static_cast<float>(X()) + m_offset_x;
		float t_y = static_cast<float>(Y());
		float t_w = static_cast<float>(W()) - m_offset_x;
		float t_h = static_cast<float>(H());

		// adjust for viewport (y is inverted)
		t_y = m_viewport_height - t_y;

		float z = 0.0f;
		float draw_x = t_x;
		float draw_y = t_y;
		float f_sz = font->getLineHeight();

		std::vector<Mesh::vertex> vertices;
		std::vector<uint32_t> v_indices;
		int v_c = 0;

		size_t i = 0;
		while (i < sentence.size()) {

			// Handle explicit newline
			if (sentence[i] == '\n') {
				if (m_multiline) {
					draw_x = t_x;
					draw_y -= f_sz;
				}
				i++;
				continue;
			}

			// Process whitespace as a single token (e.g. space, tab)
			if (isspace(static_cast<unsigned char>(sentence[i]))) {
				Glyph* glyph = font->getGlyph(static_cast<unsigned int>(sentence[i]));
				if (!glyph) {
					i++;
					continue;
				}
				// If the space glyph would exceed the line width (and we’re not already at the line start)
				if (draw_x + glyph->advance > t_x + t_w && draw_x != t_x) {
					draw_x = t_x;
					draw_y -= f_sz;
				}
				// Render the whitespace glyph
				if (draw_x >= X()) {
					const float glyph_x = draw_x + glyph->bearing_x;
					const float glyph_y = draw_y - glyph->bearing_y;
					// First triangle
					vertices.push_back({ glyph_x, glyph_y, z, glyph->left, glyph->top });
					v_indices.push_back(v_c * 4);
					vertices.push_back({ glyph_x + glyph->width, glyph_y - glyph->height, z, glyph->right, glyph->bottom });
					v_indices.push_back(v_c * 4 + 1);
					vertices.push_back({ glyph_x, glyph_y - glyph->height, z, glyph->left, glyph->bottom });
					v_indices.push_back(v_c * 4 + 2);
					// Second triangle
					v_indices.push_back(v_c * 4 + 0);
					vertices.push_back({ glyph_x + glyph->width, glyph_y, z, glyph->right, glyph->top });
					v_indices.push_back(v_c * 4 + 3);
					v_indices.push_back(v_c * 4 + 1);
					v_c++;
				}
				draw_x += glyph->advance;
				i++;
				continue;
			}

			// Process a word (a contiguous sequence of non-whitespace characters)
			size_t word_start = i;
			float word_width = 0.0f;
			size_t j = i;
			while (j < sentence.size() && !isspace(static_cast<unsigned char>(sentence[j])) && sentence[j] != '\n') {
				Glyph* glyph = font->getGlyph(static_cast<unsigned int>(sentence[j]));
				if (glyph)
					word_width += glyph->advance;
				j++;
			}

			// If the word would exceed the available width and we're not already at the beginning, break to a new line.
			if (draw_x + word_width > t_x + t_w && draw_x != t_x) {
				draw_x = t_x;
				draw_y -= f_sz;
				// NOTE, here we could add code for alignment adjustments per line.
			}

			// Render the word character by character
			for (size_t k = word_start; k < j; k++) {
				unsigned char c = static_cast<unsigned char>(sentence[k]);
				Glyph* glyph = font->getGlyph((unsigned int)c);
				if (!glyph)
					continue;

				// In the unlikely event a glyph doesn’t fit (e.g. an overly long word), break mid-word
				if (draw_x + glyph->advance > t_x + t_w) {
					if (m_multiline) {
						draw_x = t_x;
						draw_y -= f_sz;
					}
					// always draw the very frist character of a word, even if it exceeds the line width
					// this ensures there are always some vertices
					else if (!vertices.size()) {
						draw_x = t_x;
					}
					else
						break;
				}
				if (draw_x >= X()) {
					const float glyph_x = draw_x + glyph->bearing_x;
					const float glyph_y = draw_y - glyph->bearing_y;
					// First triangle
					vertices.push_back({ glyph_x, glyph_y, z, glyph->left, glyph->top });
					v_indices.push_back(v_c * 4);
					vertices.push_back({ glyph_x + glyph->width, glyph_y - glyph->height, z, glyph->right, glyph->bottom });
					v_indices.push_back(v_c * 4 + 1);
					vertices.push_back({ glyph_x, glyph_y - glyph->height, z, glyph->left, glyph->bottom });
					v_indices.push_back(v_c * 4 + 2);
					// Second triangle
					v_indices.push_back(v_c * 4 + 0);
					vertices.push_back({ glyph_x + glyph->width, glyph_y, z, glyph->right, glyph->top });
					v_indices.push_back(v_c * 4 + 3);
					v_indices.push_back(v_c * 4 + 1);
					v_c++;
				}
				draw_x += glyph->advance;
			}
			i = j;
		}

		MeshComponent* mesh_component = getComponent<MeshComponent>();
		if (mesh_component && mesh_component->getMesh()) {
			if (v_indices.empty()) {
				std::vector<Mesh::vertex> empty_vertices;
				std::vector<uint32_t> empty_indices;
				mesh_component->getMesh()->setBuffers(empty_vertices, empty_indices);
				mesh_component->getMesh()->sendEvent("update", nullptr);
				m_vertex_size = 0;
				hide();
				return;
			}

			mesh_component->getMesh()->setBuffers(vertices, v_indices);
			mesh_component->getMesh()->sendEvent("update", nullptr);
			m_vertex_size = vertices.size();
			show();
		}
	}

	Vector2 TextWidget::getCursorPosition(size_t pos)
	{
		Vector2 res;
		rhea::Font* font = getFont();
		std::string sentence = getText();
		if (!font)
			return Vector2();
		float f_sz = font->getLineHeight();
		Glyph* glyph;

		if (pos > sentence.length())
			pos = sentence.length();

		res.x = X() + m_offset_x;
		res.y = Y();
		float t_w = res.x + (float)W() - m_offset_x;

		const bool allowOverflowCursor = !m_multiline;

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
			if (allowOverflowCursor || res.x + glyph->advance < t_w) {
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
		rhea::Font* font = getFont();
		std::string sentence = getText();
		if (!font)
			return 0;
		float f_sz = font->getLineHeight();
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
		rhea::Font* font = getFont();
		if (!font)
			return 0.0f;
		std::string sentence = getText();
		float f_sz = font->getLineHeight();
		Glyph* glyph;

		for (int i = 0; i < sentence.size() && i < pos; i++) {
			glyph = font->getGlyph(static_cast<unsigned char>(sentence[i]));
			ret_x += glyph->advance;
		}
		return ret_x;
	}

	void TextWidget::setText(std::string text)
	{
		if (m_text == text)
			return;

		m_text = text;
		markTextDirty();
	}

	Vector2 TextWidget::getTextSize(Vector2 size_constraints)
	{
		rhea::Font* font = getFont();
		std::string sentence = getText();
		if (!font)
			return Vector2();

		float f_sz = font->getLineHeight();
		Vector2 size(0.0f, 0.0f);
		// At least one line height
		size.y = f_sz;
		float current_line_width = 0.0f;
		size_t i = 0;

		while (i < sentence.size()) {

			// Handle explicit newline
			if (sentence[i] == '\n') {
				if (current_line_width > size.x)
					size.x = current_line_width;
				current_line_width = 0.0f;
				// Only wrap if within vertical constraints
				if (size.y + f_sz < size_constraints.y)
					size.y += f_sz;
				i++;
				continue;
			}

			// Process whitespace as a single token (spaces, tabs, etc.)
			if (isspace(static_cast<unsigned char>(sentence[i]))) {
				Glyph* glyph = font->getGlyph(static_cast<unsigned char>(sentence[i]));
				if (!glyph) {
					i++;
					continue;
				}
				// If adding the space would exceed the horizontal constraint and we already have content on the line,
				// move to the next line (provided we have room vertically)
				if (current_line_width + glyph->advance > size_constraints.x && current_line_width > 0.0f &&
					size.y + f_sz < size_constraints.y) {
					if (m_multiline) {
						if (current_line_width > size.x)
							size.x = current_line_width;
						current_line_width = 0.0f;
						size.y += f_sz;
					}
				}
				current_line_width += glyph->advance;
				i++;
				continue;
			}

			// Process a word: a contiguous sequence of non-whitespace characters
			size_t word_start = i;
			float word_width = 0.0f;
			size_t j = i;
			while (j < sentence.size() && sentence[j] != '\n' && !isspace(static_cast<unsigned char>(sentence[j]))) {
				Glyph* glyph = font->getGlyph(static_cast<unsigned char>(sentence[j]));
				if (glyph)
					word_width += glyph->advance;
				j++;
			}

			// If the word doesn't fit on the current line and we're not at the beginning,
			// wrap to the next line (if within vertical constraints)
			if (current_line_width + word_width > size_constraints.x && current_line_width > 0.0f &&
				size.y + f_sz < size_constraints.y) {
				if (m_multiline) {
					if (current_line_width > size.x)
						size.x = current_line_width;
					current_line_width = 0.0f;
					size.y += f_sz;
				}
			}

			// If the word is too long to fit even on a fresh line, process it glyph-by-glyph
			if (word_width > size_constraints.x && current_line_width == 0.0f) {
				for (size_t k = word_start; k < j; k++) {
					Glyph* glyph = font->getGlyph(static_cast<unsigned char>(sentence[k]));
					if (!glyph)
						continue;
					if (current_line_width + glyph->advance > size_constraints.x &&
						size.y + f_sz < size_constraints.y) {
						if (m_multiline) {
							if (current_line_width > size.x)
								size.x = current_line_width;
							current_line_width = 0.0f;
							size.y += f_sz;
						}
					}
					current_line_width += glyph->advance;
				}
			}
			else {
				// Otherwise, add the whole word's width to the current line
				current_line_width += word_width;
			}
			i = j;
		}
		if (current_line_width > size.x)
			size.x = current_line_width;
		return size;
	}

	void TextWidget::setWordWrap(bool word_wrap) {
		m_word_wrap = word_wrap;
		// TODO: change appearance of text?
	}
}
