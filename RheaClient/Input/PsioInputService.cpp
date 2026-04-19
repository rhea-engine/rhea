#include "RheaClient/Input/PsioInputService.h"

namespace rhea::client
{
    PsioInputService::PsioInputService(std::string name, std::string type_name) :
        IInputService(name, type_name)
    {
        PsioInputAdapter::m_window_service = nullptr;
	}

    void PsioInputService::update()
    {
        if (!m_psioInput)
            return;

        m_psioInput->update();

        while (auto raw = m_psioInput->poll()) {
            InputEvent evt;
            if (PsioInputAdapter::translate(*raw, evt))
                m_queue.push(evt);
        }
    }

    bool PsioInputService::pollEvent(InputEvent& evt)
    {
        if (m_queue.empty())
            return false;

        evt = m_queue.front();
        m_queue.pop();
        return true;
    }
}