#include "Rhea/Input/InputMap.h"

namespace rhea
{
    void InputMap::setAction(const InputChord& chord, const std::string& action)
    {
        m_map[chord].push_back(action);
    }

    void InputMap::clearActions(const InputChord& chord)
    {
        m_map.erase(chord);
    }

    std::vector<std::string> InputMap::getActions(const InputEvent& evt) const
    {
        InputChord chord;
        chord.type = evt.type;
        chord.key = evt.key;
        chord.mouseButton = evt.mouseButton;
        chord.modifiers = evt.modifiers;

        auto it = m_map.find(chord);
        if (it == m_map.end())
            return {};

        return it->second;
    }

    void InputMap::loadDefaultMap()
    {
        if (!m_map.empty())
            return;

        setAction({ InputEventType::KeyPressed,  Key::Left }, "turn_left");
        setAction({ InputEventType::KeyPressed,  Key::Right }, "turn_right");
        setAction({ InputEventType::KeyPressed,  Key::Up }, "move_forward");
        setAction({ InputEventType::KeyPressed,  Key::Down }, "move_back");
        setAction({ InputEventType::KeyPressed,  Key::Q }, "move_left");
        setAction({ InputEventType::KeyPressed,  Key::E }, "move_right");
        setAction({ InputEventType::KeyPressed,  Key::A }, "turn_left");
        setAction({ InputEventType::KeyPressed,  Key::D }, "turn_right");
        setAction({ InputEventType::KeyPressed,  Key::W }, "move_forward");
        setAction({ InputEventType::KeyPressed,  Key::S }, "move_back");
        setAction({ InputEventType::KeyPressed,  Key::Home }, "move_up");
        setAction({ InputEventType::KeyPressed,  Key::End }, "move_down");
        setAction({ InputEventType::KeyPressed,  Key::Escape }, "cancel");
        setAction({ InputEventType::KeyPressed,  Key::Space }, "jump");
        setAction({ InputEventType::KeyPressed,  Key::F2 }, "debug");

        setAction({ InputEventType::KeyReleased, Key::Left }, "turn_left_stop");
        setAction({ InputEventType::KeyReleased, Key::Right }, "turn_right_stop");
        setAction({ InputEventType::KeyReleased, Key::Up }, "move_forward_stop");
        setAction({ InputEventType::KeyReleased, Key::Down }, "move_back_stop");
        setAction({ InputEventType::KeyReleased, Key::Q }, "move_left_stop");
        setAction({ InputEventType::KeyReleased, Key::E }, "move_right_stop");
        setAction({ InputEventType::KeyReleased, Key::A }, "turn_left_stop");
        setAction({ InputEventType::KeyReleased, Key::D }, "turn_right_stop");
        setAction({ InputEventType::KeyReleased, Key::W }, "move_forward_stop");
        setAction({ InputEventType::KeyReleased, Key::S }, "move_back_stop");
        setAction({ InputEventType::KeyReleased, Key::Home }, "move_up_stop");
        setAction({ InputEventType::KeyReleased, Key::End }, "move_down_stop");
        setAction({ InputEventType::KeyReleased, Key::F11 }, "fullscreen");
    }
}