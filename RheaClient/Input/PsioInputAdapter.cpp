#include "RheaClient/Input/PsioInputAdapter.h"
#include <Psio/Input/Keyboard.h>
#include <RheaClient/Window/WindowService.h>

namespace rhea::client
{
	WindowRegistryService* PsioInputAdapter::m_window_service = nullptr;

    Key PsioInputAdapter::translateKey(uint32_t code)
    {
        switch (code) {
        case psio::CODE_A: return Key::A;
        case psio::CODE_B: return Key::B;
        case psio::CODE_C: return Key::C;
        case psio::CODE_D: return Key::D;
        case psio::CODE_E: return Key::E;
        case psio::CODE_F: return Key::F;
        case psio::CODE_G: return Key::G;
        case psio::CODE_H: return Key::H;
        case psio::CODE_I: return Key::I;
        case psio::CODE_J: return Key::J;
        case psio::CODE_K: return Key::K;
        case psio::CODE_L: return Key::L;
        case psio::CODE_M: return Key::M;
        case psio::CODE_N: return Key::N;
        case psio::CODE_O: return Key::O;
        case psio::CODE_P: return Key::P;
        case psio::CODE_Q: return Key::Q;
        case psio::CODE_R: return Key::R;
        case psio::CODE_S: return Key::S;
        case psio::CODE_T: return Key::T;
        case psio::CODE_U: return Key::U;
        case psio::CODE_V: return Key::V;
        case psio::CODE_W: return Key::W;
        case psio::CODE_X: return Key::X;
        case psio::CODE_Y: return Key::Y;
        case psio::CODE_Z: return Key::Z;

        case psio::CODE_0: return Key::Num0;
        case psio::CODE_1: return Key::Num1;
        case psio::CODE_2: return Key::Num2;
        case psio::CODE_3: return Key::Num3;
        case psio::CODE_4: return Key::Num4;
        case psio::CODE_5: return Key::Num5;
        case psio::CODE_6: return Key::Num6;
        case psio::CODE_7: return Key::Num7;
        case psio::CODE_8: return Key::Num8;
        case psio::CODE_9: return Key::Num9;

        case psio::CODE_LEFT: return Key::Left;
        case psio::CODE_RIGHT: return Key::Right;
        case psio::CODE_UP: return Key::Up;
        case psio::CODE_DOWN: return Key::Down;
        case psio::CODE_ENTER: return Key::Enter;
        case psio::CODE_ESC: return Key::Escape;
        case psio::CODE_TAB: return Key::Tab;
        case psio::CODE_BACKSPACE: return Key::Backspace;
        case psio::CODE_SPACE: return Key::Space;
        case psio::CODE_HOME: return Key::Home;
        case psio::CODE_END: return Key::End;
        case psio::CODE_INSERT: return Key::Insert;
        case psio::CODE_DELETE: return Key::DeleteKey;
        case psio::CODE_PAGEUP: return Key::PageUp;
        case psio::CODE_PAGEDOWN: return Key::PageDown;
        case psio::CODE_F1: return Key::F1;
        case psio::CODE_F2: return Key::F2;
        case psio::CODE_F3: return Key::F3;
        case psio::CODE_F4: return Key::F4;
        case psio::CODE_F5: return Key::F5;
        case psio::CODE_F6: return Key::F6;
        case psio::CODE_F7: return Key::F7;
        case psio::CODE_F8: return Key::F8;
        case psio::CODE_F9: return Key::F9;
        case psio::CODE_F10: return Key::F10;
        case psio::CODE_F11: return Key::F11;
        case psio::CODE_F12: return Key::F12;
        case psio::CODE_SHIFT: return Key::Shift;
        case psio::CODE_CTRL: return Key::Ctrl;
        case psio::CODE_ALT: return Key::Alt;
        default: return Key::Unknown;
        }
    }

    MouseButton PsioInputAdapter::translateMouseButton(const psio::MouseInputEvent& e)
    {       
        if (e.isLeft()) return MouseButton::Left;
        if (e.isRight()) return MouseButton::Right;
        if (e.isMiddle()) return MouseButton::Middle;
        return MouseButton::None;
    }

    bool PsioInputAdapter::translate(const psio::InputEvent& src, InputEvent& dst)
    {
        dst = {};

        dst.modifiers.shift = src.isShift();
        dst.modifiers.alt = src.isAlt();
        dst.modifiers.ctrl = src.isCtrl();

        switch (src.getId()) {
        case psio::InputEventId::KEY_PRESSED: {
            auto* e = dynamic_cast<const psio::KeyInputEvent*>(&src);
            if (!e) return false;
            dst.type = InputEventType::KeyPressed;
            dst.key = translateKey(e->getKeyCode());
            dst.codepoint = e->getCode();
            return true;
        }

        case psio::InputEventId::KEY_RELEASED: {
            auto* e = dynamic_cast<const psio::KeyInputEvent*>(&src);
            if (!e) return false;
            dst.type = InputEventType::KeyReleased;
            dst.key = translateKey(e->getKeyCode());
            dst.codepoint = e->getCode();
            return true;
        }

        case psio::InputEventId::TEXT: {
            auto* e = dynamic_cast<const psio::TextInputEvent*>(&src);
            if (!e) return false;
            dst.type = InputEventType::Text;
            dst.text = e->getText();
            return true;
        }

        case psio::InputEventId::MOUSE_MOVE: {
            auto* e = dynamic_cast<const psio::MouseInputEvent*>(&src);
            if (!e) return false;
            dst.type = InputEventType::MouseMoved;
            dst.x = e->getX();
            dst.y = e->getY();
            return true;
        }

        case psio::InputEventId::MOUSE_BTN_PRESSED: {
            auto* e = dynamic_cast<const psio::MouseInputEvent*>(&src);
            if (!e) return false;
            dst.type = InputEventType::MouseButtonPressed;
            dst.x = e->getX();
            dst.y = e->getY();
            dst.mouseButton = translateMouseButton(*e);
            return true;
        }

        case psio::InputEventId::MOUSE_BTN_RELEASED: {
            auto* e = dynamic_cast<const psio::MouseInputEvent*>(&src);
            if (!e) return false;
            dst.type = InputEventType::MouseButtonReleased;
            dst.x = e->getX();
            dst.y = e->getY();
            dst.mouseButton = translateMouseButton(*e);
            return true;
        }

        case psio::InputEventId::MOUSE_BTN_DBLCLICK: {
            auto* e = dynamic_cast<const psio::MouseInputEvent*>(&src);
            if (!e) return false;
            dst.type = InputEventType::MouseButtonDoubleClicked;
            dst.x = e->getX();
            dst.y = e->getY();
            dst.mouseButton = translateMouseButton(*e);
            return true;
        }

        case psio::InputEventId::WMGR_QUIT:
            dst.type = InputEventType::WindowQuit;
            return true;

        case psio::InputEventId::WIN_SIZE: {
            auto* e = dynamic_cast<const psio::WinSizeEvent*>(&src);
            if (!e) return false;
            dst.type = InputEventType::WindowResized;
            dst.width = static_cast<int>(const_cast<psio::WinSizeEvent*>(e)->width());
            dst.height = static_cast<int>(const_cast<psio::WinSizeEvent*>(e)->height());
            psio::Window* win = const_cast<psio::WinSizeEvent*>(e)->getWindow();
            if (win && m_window_service) {
               dst.window = m_window_service->getWindowId(win);
            }

            return true;
        }

        default:
            return false;
        }
    }
}