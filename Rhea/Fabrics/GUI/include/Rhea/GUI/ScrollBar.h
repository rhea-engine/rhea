#ifndef RHEA_GUI_SCROLLBAR_H
#define RHEA_GUI_SCROLLBAR_H

#include <Rhea/GUI/Widget.h>

namespace rhea
{
    // ScrollBar.h
    class ScrollBar : public Widget {
        RECLASS(ScrollBar, Widget, "//meworla/scrollbar");
    public:
        enum class Orientation { Vertical, Horizontal };

		ScrollBar(std::string name, std::string type_name) :
			m_track(nullptr),
			m_thumb(nullptr),
            m_dragging(false),
			m_orientation(Orientation::Vertical),
			Widget(name, type_name)
		{}

        void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

        void ready() override;

        // Set thumb size (0.0 - 1.0) and position (0.0 - 1.0)
        void setThumbPosition(float position, float size_ratio);

        // Callback when user drags thumb or clicks track
        void setOnScroll(std::function<void(float)> callback);

		void onSetPosition() override;
        Vector2 innerSize(Vector2 constraints) override;

        const std::string getSelector() { return "ScrollBar"; }

    private:
		Widget* m_track;      // Background track
        Widget* m_thumb;      // Movable thumb
        Orientation m_orientation;
        std::function<void(float)> m_on_scroll;
        bool m_dragging;
        float m_drag_offset = 0.0f;

		float m_thumb_position = 0.0f; // Current thumb position (0.0 - 1.0)

        bool onMouseClicked(Event* ev, Unit* sender) override;
        bool onDragMove(Event* ev, Unit* sender) override;        
		bool onDragStart(Event* ev, Unit* sender) override;
		bool onDragEnd(Event* ev, Unit* sender) override;
    };
}

#endif // !RHEA_GUI_SCROLLBAR_H