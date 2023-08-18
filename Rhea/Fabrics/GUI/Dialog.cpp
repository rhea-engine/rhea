#include "Dialog.h"
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>

#include "Modifier/FillSize.h"

namespace Rhea
{
	void Dialog::ready()
	{
		Widget::ready();

		registerEventHandler("close", this, [&](Event* e, Unit* u)->bool {
			hide();
			return false;
			});
	}

	void Dialog::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Widget::setup(app, attributes, values);

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		// dialogs are not visible by default and must be "opened"
		m_visible = false;
	}

	// a dialog centers itself in the parents width 
	void Dialog::onSetPosition()
	{
		Widget* p = doCast<Widget>(m_parent);
		if (p) {
			Vector2 parent_size = p->getSize();

			// m_size contains the size parameter or the size needed by its children
			Vector2 padding = Vector2((parent_size.x - m_size.x) / 2, (parent_size.y - m_size.y) / 2);
			m_position += padding;
		}

		// set children position
		Widget::onSetPosition();
	}
}