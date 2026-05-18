/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#pragma once

#include <Psio/Input/InputEvent.h>
#include <Psio/Input/Keyboard.h>

#include <memory>
#include <cstdint>

namespace psio
{
    std::shared_ptr<InputEvent> BuildQuitEvent();

    std::shared_ptr<KeyInputEvent> BuildKeyEvent(
        InputEventId id,
        uint32_t keycode,
        uint32_t physicalKeycode,
        uint8_t unicode,
        bool shift,
        bool alt,
        bool ctrl);

    std::shared_ptr<TextInputEvent> BuildTextEvent(
        std::string text,
        bool shift,
        bool alt,
        bool ctrl);

    std::shared_ptr<MouseInputEvent> BuildMouseEvent(
        InputEventId id,
        int x,
        int y,
        bool left,
        bool right,
        bool middle,
        bool shift,
        bool alt,
        bool ctrl);

    std::shared_ptr<MouseWheelEvent> BuildMouseWheelEvent(
        InputEventId id,
        int wheelY,
        int wheelX,
        int x,
        int y,
        bool shift,
        bool alt,
        bool ctrl);
}