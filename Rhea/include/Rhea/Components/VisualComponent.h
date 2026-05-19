/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_VISUALCOMPONENT_H
#define RHEA_VISUALCOMPONENT_H

#include <Rhea/Component.h>

namespace rhea
{
    struct ClipRect {
        float x;
        float y;
        float w;
        float h;
    };

    /** Manages visibility state only */
    class VisualComponent : public Component
    {
        RECLASS(VisualComponent, Component, "//meworla/visualcomponent")
    public:
        VisualComponent(std::string name, std::string type_name) :
            Component(name, type_name),
            m_hidden(false)
        {}

        // Visibility management
        void hide() { m_hidden = true; }
        void show() { m_hidden = false; }
        bool isVisible() const { return !m_hidden; }
		void setVisible(bool visible) { m_hidden = !visible; }

        void enableClipping(bool enable) { m_has_clipping = enable; }
        bool hasClipping() { return m_has_clipping; }
        ClipRect getClipping() { return m_clip_rect; }
        void setClipping(ClipRect rect) { m_clip_rect = rect; }

    private:
        bool m_hidden;
        bool m_has_clipping = false;
        ClipRect m_clip_rect{};
    };
}
#endif
