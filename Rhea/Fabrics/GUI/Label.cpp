#include "Label.h"
#include "Application.h"
#include "UnitTree.h"
#include "Fabrics/Core/Scene.h"
#include "../Text/TextWidget.h"
#include <Psio/Log.h>

namespace Rhea
{
	void Label::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		m_templating = getValue("templating", m_templating);

		val["x"] = m_base_position.x;
		val["y"] = m_base_position.y;
		val["w"] = m_base_size.x;
		val["h"] = m_base_size.y;
		val["text"] = getValue("text", "");
		if (m_values.find("font") != m_values.end())
			val["font"] = getValue("font");
		val["word_wrap"] = getValue("word_wrap", true);
		val["multiline"] = getValue("multiline", true);

		// DEBUG
		// always handle input 
		m_handle_input = true;

		if (m_templating) {
			parseText(val["text"]);
			val["text"] = getText();
		}

		m_label_item = (TextWidget*)m_app->getUnitTree()->createUnit(m_path + "/label", "//meworla/textwidget", attr, val);
		addChild(m_label_item);
	}

	Label* Label::createType(std::string name, std::string type_name)
	{
		Label* ret = new Label(name, type_name);
		return ret;
	}

	void Label::setText(std::string text)
	{
		m_values["text"] = text; 
		if (m_templating) {
			parseText(text);
			text = getText();
		}		
		m_label_item->setText(text);
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
					m_textElements.push_back({ 0, text.substr(start, reader - start) });
					start = reader + 1;
					state = 1;
					path = "";
				}
				break;
			case 1:
				if (text[reader] == '}') {
					if (start < reader)
						m_textElements.push_back({ 1, "", text.substr(start, reader - start) });
					start = reader + 1;
					state = 0;
				}
				else if (text[reader] == '.') {
					if (start < reader)
						path = text.substr(start, reader - start);
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
				std::string path = m_attributes["basepath"] + "/" + it->path;
				Unit* u = m_app->getUnitTree()->find(path);
				if (u != nullptr)
					res += u->getValue(it->value, "");
				break;
			}
		}
		return res;
	}

	void Label::onFrame(float delta)
	{
		if (m_templating) {
			std::string t = getText();
			if (t.compare(m_label_item->getText())) {
				m_label_item->setText(t);
			}
		}
	}

	void Label::setArea(bool moved, bool sized)
	{
		Widget::setArea(moved, sized);

		if (m_label_item) {			
			if (moved) {
				DLOG(LOG_DEBUG) << "GUI::Label: widget " << m_pathname << ": screen(x,y)=" << m_position.x << ", " << m_position.y << ")";
				m_label_item->move(m_position.x, m_position.y);
			}
			if (sized) {
				DLOG(LOG_DEBUG) << "GUI::Label: widget " << m_pathname << ": size(w,h)" << m_size.x << ", " << m_size.y << ")";
				m_label_item->resize(m_size.x, m_size.y);
			}
			if (sized || moved) {
				m_label_item->update();
			}
		}
	}

	void Label::ready()
	{
		Widget::ready();

		// find the scene
		Scene* scene = (Scene*)m_app->getUnitTree()->findByType("//meworla/scene");
		if (!scene) {
			LOG(LOG_ERROR) << "Widget: Scene not found";
			return;
		}

		if (m_label_item) {
			m_label_item->ready();
			//scene->addItem((Item*)m_label_item);
		}
	}
}