#include <functional>
#include <algorithm>

#include <Poseidon/Entity.h>

#include <Rhea/Fabrics/Text/TextWidget.h>
#include "Application.h"

#include "TextManager.h"
#include "RenderElement.h"

namespace Rhea
{
	Element* GraphicsTextManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		// create the element
		GraphicsRenderElement* elem = new GraphicsRenderElement(core);
		Poseidon::Node* entity = createRenderable(elem, core, unit, "HUD");

		// add the position
		addPosition(unit, entity);
		
		// material (the font bitmap)
		Rhea::Font* font = static_cast<TextWidget*>(unit)->getFont();
		if (!font) {
			unit->getApplication()->log("Text has no font");
			delete elem;
			return nullptr;
		}
		Rhea::Material* material = font->getMaterial();
		if (!material) {
			unit->getApplication()->log("Text has no material");	
			delete elem;
			return nullptr;
		}
		setMaterial(core, material, entity);
		unit->addElement("graphics", elem);
		
		// register handlers
		unit->registerEventHandler("delete", std::bind(&GraphicsTextManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		unit->registerEventHandler("update", std::bind(&GraphicsTextManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));

		// add to my list
		m_elements.push_back(elem);

		// inital update
		setText(static_cast<TextWidget*>(unit), elem);

		return elem;
	}

	void GraphicsTextManager::setText(TextWidget* t, GraphicsRenderElement* elem)
	{
		std::vector<Hippios::vertex_uv> vertices;
		std::vector<unsigned long> v_indices;

		Rhea::Font* font = t->getFont();
		std::string sentence = t->getText();
		float t_x = (float)t->X();
		float t_y = (float)t->Y();
		float t_w = (float)t->W();
		float t_h = (float)t->H();	
		
		float z = 0.0f;
		float draw_x = t_x;
		float draw_y = t_y;
		Glyph* glyph;
		float f_sz = font->getFontSize();

		for (int i = 0; i < sentence.size(); i++) {

			glyph = font->getGlyph((unsigned int)sentence[i]);

			// safety check
			if (!glyph)
				return;

			// top/left=0/0
			float f_left = glyph->left;
			float f_right = glyph->right;
			float f_top = glyph->top;
			float f_bottom = glyph->bottom;

			// do we have space for the next char?
			if (draw_x + glyph->advance > t_w - t_x) {
				// do we have space for the next line?
				if (draw_y - (2 * f_sz) < t_y - t_h) {
					// no - stop rendering
					break;
				}
				// go to next line
				draw_x = t_x;
				draw_y -= f_sz;
			}

			// first triangle
			// top-left
			vertices.push_back({ draw_x, draw_y, z, f_left, f_top });
			v_indices.push_back(i * 4);
			// bottom-right
			vertices.push_back({ draw_x + glyph->advance, draw_y - f_sz, z, f_right, f_bottom });
			v_indices.push_back(i * 4 + 1);
			// bottom-left
			vertices.push_back({ draw_x, draw_y - f_sz, z, f_left, f_bottom });
			v_indices.push_back(i * 4 + 2);

			// second triangle
			// top-left
			v_indices.push_back(i * 4 + 0);
			// top-right
			vertices.push_back({ draw_x + glyph->advance, draw_y, z, f_right, f_top });
			v_indices.push_back(i * 4 + 3);
			// bottom-right
			v_indices.push_back(i * 4 + 1);

			// advance by glyph width
			draw_x += glyph->advance;
		}

		elem->getMesh()->setVertices(sizeof(Hippios::vertex_uv), vertices.size(), vertices.data());
		elem->getMesh()->setIndices(sizeof(unsigned long), v_indices.size() / 3, v_indices.data());
	}

	bool GraphicsTextManager::onUpdate(Event* ev, Unit* unit)
	{
		printf("text updated\n");

		TextWidget* t = static_cast<TextWidget*>(unit);
		GraphicsRenderElement* elem = static_cast<GraphicsRenderElement*>(t->getElement("graphics"));

		setText(t, elem);

		return false;
	}

	void GraphicsTextManager::onFrame()
	{
	}
	
	bool GraphicsTextManager::onItemDelete(Event* ev, Unit* unit)
	{
		//erase element from list
		Element* elem = unit->getElement("graphics");
		auto elem_pos = std::find(m_elements.begin(), m_elements.end(), elem);
		if (elem_pos != m_elements.end())
			m_elements.erase(elem_pos);
		unit->addElement("graphics", nullptr);
		// free element
		delete elem;

		return false;
	}
}