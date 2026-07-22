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

    enum class RenderPhase
    {
        Background,
        Opaque,
        Transparent,
        Overlay
    };

    /** Manages visual properties and rendering phases */
    class VisualComponent : public Component
    {
        RECLASS(VisualComponent, Component, "//meworla/visualcomponent");
        REPROPERTY(bool, "hidden", Label("Hidden"), Edit, Read);
        REPROPERTY(bool, "has_clipping", Label("Has Clipping"), Edit, Read);
        REPROPERTY(ClipRect, "clip_rect", Label("Clip Rect"), Edit, Read);
        REPROPERTY(RenderPhase, "render_phase", Label("Render Phase"), Edit, Read);
    public:
        VisualComponent(std::string name, std::string type_name) :
            Component(name, type_name),
            m_hidden(false),
            m_render_phase(RenderPhase::Opaque)
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

		RenderPhase getRenderPhase() { return m_render_phase; }
		void setRenderPhase(RenderPhase phase) { m_render_phase = phase; }

    private:
       
        bool m_hidden;
       
        bool m_has_clipping = false;
		
        ClipRect m_clip_rect{};
        RenderPhase m_render_phase;
    };
}
#endif
