#include "Rhea/GUI/ScrollBar.h"
#include <Rhea/EventInput.h>
#include <algorithm>
#include <Psio/Log.h>

namespace rhea {

	void ScrollBar::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) {
		Widget::setup(app, attributes, values);
		// Set orientation if specified
		if (attributes.find("orientation") != attributes.end()) {
			std::string orientation = attributes["orientation"];
			if (orientation == "horizontal") {
				m_orientation = Orientation::Horizontal;
			}
			else {
				m_orientation = Orientation::Vertical;
			}
		}
	}

	void ScrollBar::ready() {
		Widget::ready();
		// Create track and thumb
        m_track = new Widget(getPathName() + "_track");
        m_track->setup(m_app, {}, {	
			});		
        m_thumb = new Widget(getPathName() + "_thumb");
        m_thumb->setup(m_app, {}, {			
			});
		addChild(m_track);
		addChild(m_thumb);
        m_track->ready();
        m_thumb->ready();
	}

	void ScrollBar::setThumbPosition(float position, float size_ratio) {
		// Clamp position and size_ratio to [0.0, 1.0]
		position = std::clamp(position, 0.0f, 1.0f);
		size_ratio = std::clamp(size_ratio, 0.0f, 1.0f);
		m_thumb_position = position;
		// Set thumb size based on size_ratio
		if (m_orientation == Orientation::Vertical) {
			m_thumb->resize(m_track->W(), m_track->H() * size_ratio);
			m_thumb->setPosition(m_position + Vector2(0, m_track->H() * position));
		}
		else {
			m_thumb->resize(m_track->W() * size_ratio, m_track->H());
			m_thumb->setPosition(m_position + Vector2(m_track->W() * position, 0));
		}
	}

	void ScrollBar::setOnScroll(std::function<void(float)> callback) {		
		m_on_scroll = callback;
	}

	void ScrollBar::onSetPosition() {
		// Update track and thumb positions
		m_track->setPosition(m_position);
		if (m_orientation == Orientation::Vertical) {
			m_thumb->setPosition(m_position + Vector2(0, m_track->H() * m_thumb_position));
		}
		else {
			m_thumb->setPosition(m_position + Vector2(m_track->W() * m_thumb_position, 0));
		}
	}

	Vector2 ScrollBar::innerSize(Vector2 constraints) {		
		// ScrollBar takes all available space
		m_track->setSize(constraints);
		if (m_orientation == Orientation::Vertical) {
			m_thumb->setSize(Vector2(constraints.x, m_thumb->H())); // Keep thumb height as is
		}
		else {
			m_thumb->setSize(Vector2(m_thumb->W(), constraints.y)); // Keep thumb width as is
		}
		return constraints;
	}

    bool ScrollBar::onMouseClicked(Event* ev, Unit* sender) {
        EventInput* input = static_cast<EventInput*>(ev);
		float mouse_x = input->getEvent().x;
		float mouse_y = input->getEvent().y;
		
        // Check if clicked on thumb
        if (m_thumb == capture(mouse_x, mouse_y)) {
			return true;
        }

        // Clicked on track - jump to position
        float track_y = mouse_y - m_track->Y();
        float position = track_y / m_track->H();
        if (m_on_scroll) m_on_scroll(position);
        return true;
    }

	bool ScrollBar::onDragStart(Event* ev, Unit* sender) {
		EventInput* input = static_cast<EventInput*>(ev);
		float mouse_x = input->getEvent().x;
		float mouse_y = input->getEvent().y;

		// Check if dragging starts on thumb
		if (m_thumb == capture(mouse_x, mouse_y)) {
			m_dragging = true;
			m_drag_offset = mouse_y - m_thumb->Y();
			return true;
		}
		return false;
	}

    bool ScrollBar::onDragMove(Event* ev, Unit* sender) {
        if (!m_dragging) return false;

        EventInput* input = static_cast<EventInput*>(ev);
        float mouse_y = input->getEvent().y;
        float new_thumb_y = mouse_y - m_drag_offset;

		// clamp inside of track
		new_thumb_y = std::clamp(new_thumb_y, m_track->Y(), m_track->Y() + m_track->H() - m_thumb->H());

        // Convert to 0.0 - 1.0 position
        float position = (new_thumb_y - m_track->Y()) / (m_track->H() - m_thumb->H());
        position = std::clamp(position, 0.0f, 1.0f);

        if (m_on_scroll) m_on_scroll(position);
        return true;
    }

    bool ScrollBar::onDragEnd(Event* ev, Unit* sender) {
        if (m_dragging) {
            m_dragging = false;
            return true;
        }
        return false;
    }
}