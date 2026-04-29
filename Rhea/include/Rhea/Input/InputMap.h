/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_INPUT_INPUTMAP_H
#define RHEA_INPUT_INPUTMAP_H

#include "InputEvent.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <cstddef>

namespace rhea
{
    struct InputChord
    {
        InputEventType type = InputEventType::None;
        Key key = Key::Unknown;
        MouseButton mouseButton = MouseButton::None;
        Modifiers modifiers;

        bool operator==(const InputChord& other) const {
            return type == other.type
                && key == other.key
                && mouseButton == other.mouseButton
                && modifiers == other.modifiers;
        }
    };

    struct InputChordHash
    {
        std::size_t operator()(const InputChord& c) const
        {
            std::size_t h = 0;

            auto mix = [&h](std::size_t v) {
                h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2);
                };

            mix(std::hash<int>()(static_cast<int>(c.type)));
            mix(std::hash<int>()(static_cast<int>(c.key)));
            mix(std::hash<int>()(static_cast<int>(c.mouseButton)));
            mix(std::hash<bool>()(c.modifiers.shift));
            mix(std::hash<bool>()(c.modifiers.alt));
            mix(std::hash<bool>()(c.modifiers.ctrl));

            return h;
        }
    };

    class InputMap
    {
    public:
        void setAction(const InputChord& chord, const std::string& action);
        void clearActions(const InputChord& chord);
        std::vector<std::string> getActions(const InputEvent& evt) const;

        void loadDefaultMap();

    protected:
        std::unordered_map<InputChord, std::vector<std::string>, InputChordHash> m_map;
    };
}

#endif