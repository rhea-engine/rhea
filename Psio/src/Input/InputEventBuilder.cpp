#include "InputEventBuilder.h"

namespace Psio
{
    std::shared_ptr<InputEvent> BuildQuitEvent()
    {
        return InputEvent::createEvent(InputEventId::WMGR_QUIT);
    }

    std::shared_ptr<KeyInputEvent> BuildKeyEvent(
        InputEventId id,
        uint32_t keycode,
        uint32_t physicalKeycode,
        uint8_t unicode,
        bool shift,
        bool alt,
        bool ctrl)
    {
        auto evt = std::static_pointer_cast<KeyInputEvent>(InputEvent::createEvent(id));
        evt->set(keycode, physicalKeycode, unicode);
        evt->setModifiers(shift, alt, ctrl);
        return evt;
    }

    std::shared_ptr<TextInputEvent> BuildTextEvent(
		std::string text,   
        bool shift,
        bool alt,
        bool ctrl)
    {
        auto evt = std::static_pointer_cast<TextInputEvent>(InputEvent::createEvent(InputEventId::TEXT));
		evt->setText(text);
        evt->setModifiers(shift, alt, ctrl);
        return evt;
	}

    std::shared_ptr<MouseInputEvent> BuildMouseEvent(
        InputEventId id,
        int x,
        int y,
        bool left,
        bool right,
        bool middle,
        bool shift,
        bool alt,
        bool ctrl)
    {
        auto evt = std::static_pointer_cast<MouseInputEvent>(InputEvent::createEvent(id));
        evt->setPos(x, y);
        evt->setBtn(left, right, middle);
        evt->setModifiers(shift, alt, ctrl);
        return evt;
    }
}