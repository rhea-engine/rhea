#ifndef RHEA_INPUT_INPUTEVENT_H
#define RHEA_INPUT_INPUTEVENT_H

#include <string>
#include <cstdint>
#include <Rhea/Graphics/Window.h>

namespace rhea
{
    enum class InputEventType {
        None,
        KeyPressed,
        KeyReleased,
        Text,
        MouseMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseButtonDoubleClicked,
        WindowQuit,
        WindowResized
    };

    enum class Key {
        Unknown,

        A, B, C, D, E, F, G,
        H, I, J, K, L, M, N,
        O, P, Q, R, S, T, U,
        V, W, X, Y, Z,

        Num0, Num1, Num2, Num3, Num4,
        Num5, Num6, Num7, Num8, Num9,

        Left, Right, Up, Down,
        Enter, Escape, Tab, Backspace, Space,
        Home, End, Insert, DeleteKey,
        PageUp, PageDown,

        F1, F2, F3, F4, F5, F6,
        F7, F8, F9, F10, F11, F12,

        Shift,
        Ctrl,
        Alt
    };

    enum class MouseButton {
        None,
        Left,
        Right,
        Middle
    };

    struct Modifiers {
        bool shift = false;
        bool alt = false;
        bool ctrl = false;

        bool operator==(const Modifiers& other) const {
            return shift == other.shift
                && alt == other.alt
                && ctrl == other.ctrl;
        }
    };

    struct InputEvent
    {
        InputEventType type = InputEventType::None;

        Key key = Key::Unknown;
        MouseButton mouseButton = MouseButton::None;
        Modifiers modifiers;

        int x = 0;
        int y = 0;
        int dx = 0;
        int dy = 0;

        int wheelX = 0;
        int wheelY = 0;

        std::string text;
        uint32_t codepoint = 0;

		WindowId window = 0;

        int width = 0;
        int height = 0;

        bool isKeyPressed(Key k) const {
            return type == InputEventType::KeyPressed && key == k;
        }

        bool isKeyReleased(Key k) const {
            return type == InputEventType::KeyReleased && key == k;
        }

        bool isTextEntered() const {
            return type == InputEventType::Text;
        }

        bool isMouseMoved() const {
            return type == InputEventType::MouseMoved;
        }

        bool isMousePressed(MouseButton btn) const {
            return type == InputEventType::MouseButtonPressed && mouseButton == btn;
        }

        bool isMouseReleased(MouseButton btn) const {
            return type == InputEventType::MouseButtonReleased && mouseButton == btn;
        }

        bool isQuit() const {
            return type == InputEventType::WindowQuit;
        }
    };
}

#endif