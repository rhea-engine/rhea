#ifndef RHEA_VISUALCOMPONENT_H
#define RHEA_VISUALCOMPONENT_H

#include <Rhea/Component.h>

namespace rhea
{
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

    private:
        bool m_hidden;
    };
}
#endif
