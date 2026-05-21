/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/Label.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Scene.h>
#include <Rhea/Text/TextWidget.h>
#include <Psio/Log.h>

namespace rhea
{
	void Label::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		m_is_renderable = false;

		m_templating = getValue("templating", m_templating);

		val["x"] = m_base_position.x;
		val["y"] = m_base_position.y;
		val["text"] = getValue("text", "");
		if (m_values.find("font") != m_values.end()) {
			val["font"] = getValue("font");
			m_properties[StylePropertyId::kFont].push_back({ CssValueId::kString, getValue("font") });
		}
		val["word_wrap"] = getValue("word_wrap", true);
		val["multiline"] = getValue("multiline", true);
		m_clip = getValue("clip", m_clip);

		m_computed_properties = m_properties;

		if (m_templating) {
			parseText(val["text"].toString());
			val["text"] = getText();
		}

		m_label_item = (TextWidget*)m_app->getUnitTree()->createUnit(getPathName() + "_label", "//meworla/textwidget", attr, val);
		addChild(m_label_item);
	}

	void Label::setText(std::string text)
	{
		m_values["text"] = text; 
		if (m_templating) {
			parseText(text);
			text = getText();
		}		
		if (m_label_item)
			m_label_item->setText(text);
		else
			LOG(LOG_ERROR) << "Label: No text widget found to set text: Did you forget to call setup?";

		invalidateLayout();
	}

	Vector2 Label::innerSize(Vector2 size_constraints)
	{
		// get height from font of text widget
		Vector2 child_size = m_label_item->getTextSize(size_constraints);
		// fill label to my size
		if (m_size.x > child_size.x) {
			child_size.x = m_size.x;
		}
		if (m_size.y > child_size.y) {
			child_size.y = m_size.y;
		}

		// clipping?
		if (m_clip) {
			if (child_size.x > size_constraints.x)
				child_size.x = size_constraints.x;
			if (child_size.y > size_constraints.y)
				child_size.y = size_constraints.y;
		}

		return child_size;
	}

	void Label::innerGetMinSize(Vector2& min_size)
	{
		Vector2 child_size = m_label_item->getTextSize(min_size);
		if (min_size.x < child_size.x)
			min_size.x = child_size.x;
		if (min_size.y < child_size.y)
			min_size.y = child_size.y;
	}

	void Label::onSetPosition()
	{
		if (!m_label_item)
			return;

		// set clipping area for text widget
		auto* vis = getComponent<VisualComponent>();
		auto* t_vis = m_label_item->getComponent<VisualComponent>();
		if (vis && t_vis) {
			t_vis->enableClipping(vis->hasClipping());
			if (vis->hasClipping()) {
				t_vis->setClipping(vis->getClipping());
			}
		}
			
			float pos_x = floor(m_position.x + m_content_position.x + 0.5f);
		float pos_y = floor(m_position.y + m_content_position.y + 0.5f);

		bool size_changed = (m_label_item->W() != m_size.x || m_label_item->H() != m_size.y);
		bool pos_changed = (m_label_item->X() != pos_x || m_label_item->Y() != pos_y);

			
			// prevent Text Rendering at Fractional Coordinates
		//	float pos_x = floor(m_position.x + m_content_position.x + 0.5f);
		//	float pos_y = floor(m_position.y + m_content_position.y + 0.5f);

		if (pos_changed) {
			m_label_item->move(pos_x, pos_y);
			m_label_item->markTextDirty();
		}
		if (size_changed) {
			m_label_item->resize(m_size.x, m_size.y);
			m_label_item->markTextDirty(); // wrapping area changed
		}
	}

	void Label::ApplyStyles()
	{
		Widget::ApplyStyles();

		if (m_label_item) {

			// set the font style
			std::vector<CssValue> font_prop = getStyle(StylePropertyId::kFont);
			if (font_prop.size()) {
				Font* font = doCast<Font>(m_app->getUnitTree()->find(font_prop[0].value.toString()));
				if (font != m_label_item->getFont()) {
					if (!font)
						LOG(LOG_ERROR) << "font " << (std::string)(font_prop[0].value) << " not found";
					else {
						m_label_item->setFont(font); // local text dirty only
						invalidateLayout();			 // font can change measured size
					}
				}
			}
			else
				LOG(LOG_ERROR) << "No font style found for label";
		}

	}

	void Label::parseText(const std::string text)
	{
		m_textElements.clear();
		size_t reader = 0;
		size_t start = 0;
		int state = 0;
		std::string path;

		size_t len = text.length();

		while (reader < len) {
			switch (state) {
			case 0:
				if (text[reader] == '{') {
					// up to now we have a normal string
					if (start < reader)
						m_textElements.push_back({ 0, text.substr(start, reader - start) });
					start = reader + 1;
					state = 1;
					path = "";
				}
				break;
			case 1:
				if (text[reader] == '}') {
					if (start < reader)
						m_textElements.push_back({ 1, text.substr(start, reader - start), m_attributes["basepath"] });
					start = reader + 1;
					state = 0;
				}
				else if (text[reader] == '.') {
					if (start < reader)
						path = m_attributes["basepath"] + "/" + text.substr(start, reader - start);
					start = reader + 1;
					state = 2;
				}
				break;
			case 2:
				if (text[reader] == '}') {
					if (start < reader)
						m_textElements.push_back({ 1, text.substr(start, reader - start), path });
					start = reader + 1;
					state = 0;
				}
				break;
			}
			reader++;
		}
		if (start < len)
			m_textElements.push_back({ 0, text.substr(start) });
	}

	std::string Label::getText()
	{
		std::string res;
		for (auto it = m_textElements.begin(); it != m_textElements.end(); ++it) {
			switch (it->type) {
			case 0:
				res += it->value;
				break;
			case 1:				
				Unit* u = doCast<Unit>(m_app->getUnitTree()->find(it->path));
				if (u != nullptr)
					res += u->getValueString(it->value, "");
				break;
			}
		}
		return res;
	}

	void Label::onFrame(float delta)
	{
		if (m_templating) {
			std::string t = getText();
			if (m_label_item && t.compare(m_label_item->getText())) {
				m_label_item->setText(t);
			}
		}
	}

	void Label::ready()
	{
		Widget::ready();
	}
}