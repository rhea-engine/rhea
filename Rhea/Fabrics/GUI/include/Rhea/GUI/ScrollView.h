#ifndef RHEA_GUI_SCROLLVIEW_H
#define RHEA_GUI_SCROLLVIEW_H

#include "Widget.h"
#include "ScrollBar.h"

namespace rhea
{
	class ScrollView : public Widget
	{
		RECLASS(ScrollView, Widget, "//meworla/gui/scrollview")
	public:
		ScrollView(std::string path_name, std::string type_name) :
			Widget(path_name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;
		
		void ready() override;

		void onSetPosition() override;
		Vector2 innerSize(Vector2 constraints) override;

		bool onMouseWheel(Event* ev, Unit* unit) override;

		void invalidateLayout() override;

		void setScrollY(float y);
		void ensureVisible(Widget* child);

		// styling
		const std::string getSelector() { return "ScrollView"; }

	private:
		Widget* m_content = nullptr;
		float m_scroll_y = 0.0f;
		float m_max_scroll_y = 0.0f;

	protected:
		ScrollBar* m_scrollbar = nullptr;
		float m_scrollbar_width = 16.0f;
		Vector2 m_scroll_offset;
		Vector2 m_child_content_size;
		float m_scroll_distance = 20.0f;
	};
}
#endif // !RHEA_GUI_SCROLLVIEW_H
