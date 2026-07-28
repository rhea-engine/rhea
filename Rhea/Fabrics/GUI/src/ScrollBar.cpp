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
		m_track->setSelector("ScrollBarTrack");
        m_track->setup(m_app, {}, {	
			});		
        m_thumb = new Widget(getPathName() + "_thumb");
		m_thumb->setSelector("ScrollBarThumb");
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
			m_thumb->invalidateRender();
		}
		else {
			m_thumb->resize(m_track->W() * size_ratio, m_track->H());
			m_thumb->setPosition(m_position + Vector2(m_track->W() * position, 0));
			m_thumb->invalidateRender();
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

	void ScrollBar::invalidateLayout() {
		Widget::invalidateLayout();

		// also invalidate track and thumb
		if (m_track) m_track->invalidateLayout();
		if (m_thumb) m_thumb->invalidateLayout();
	}

    bool ScrollBar::onMouseClicked(Event* ev, Unit* sender) {
		ev->stopPropagation();
        EventInput* input = static_cast<EventInput*>(ev);
		float mouse_x = input->getEvent().x;
		float mouse_y = input->getEvent().y;
		
        // Check if clicked on thumb
        if (m_thumb == capture(mouse_x, mouse_y)) {
			return true;
        }

        // Clicked on track - page up/down
		float movable = m_track->H() - m_thumb->H();
		if (movable <= 0.0f) {
			if (m_on_scroll) m_on_scroll(0.0f);
			return true;
		}

		float page_step = m_thumb->H() / movable;
		float position = m_thumb_position;

		if (mouse_y < m_thumb->Y()) {
			position -= page_step;
		}
		else if (mouse_y > m_thumb->Y() + m_thumb->H()) {
			position += page_step;
		}

		if (m_on_scroll)
			m_on_scroll(std::clamp(position, 0.0f, 1.0f));

		// Note: could add callback to scrollview for correct page-size scrolling, but for now just call on_scroll with new position

        return true;
    }

	bool ScrollBar::onDragStart(Event* ev, Unit* sender) {
		EventInput* input = static_cast<EventInput*>(ev);
		float mouse_x = input->getEvent().x;
		float mouse_y = input->getEvent().y;

		// Check if dragging starts on thumb
		if (m_thumb == capture(mouse_x, mouse_y)) {
			// safety check if the thumb is not movable (e.g., content fits in view)
			float movable = m_track->H() - m_thumb->H();
			if (movable <= 0.0f) {
				return false;
			}
			ev->stopPropagation();
			m_dragging = true;
			m_drag_offset = mouse_y - m_thumb->Y();
			return true;
		}
		return false;
	}

    bool ScrollBar::onDragMove(Event* ev, Unit* sender) {
        if (!m_dragging) return false;

		ev->stopPropagation();
        EventInput* input = static_cast<EventInput*>(ev);
        float mouse_y = input->getEvent().y;
        float new_thumb_y = mouse_y - m_drag_offset;

		// safety check if the thumb is not movable (e.g., content fits in view)
		float movable = m_track->H() - m_thumb->H();
		if (movable <= 0.0f) {
			m_dragging = false;
			return false;
		}

		// clamp inside of track
		new_thumb_y = std::clamp(new_thumb_y, m_track->Y(), m_track->Y() + movable);

        // Convert to 0.0 - 1.0 position
        float position = (new_thumb_y - m_track->Y()) / movable;
        position = std::clamp(position, 0.0f, 1.0f);

        if (m_on_scroll) m_on_scroll(position);
        return true;
    }

    bool ScrollBar::onDragEnd(Event* ev, Unit* sender) {
        if (m_dragging) {
			ev->stopPropagation();
            m_dragging = false;
            return true;
        }
        return false;
    }
}