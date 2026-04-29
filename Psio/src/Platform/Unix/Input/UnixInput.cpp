/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "UnixInput.h"
#include <Psio/Input/InputEvent.h>

namespace psio
{
    void InputUnix::update()
    {
        // TODO: Implement Unix input handling
        // This could use:
        // - X11 event polling (XPending, XNextEvent)
        // - Wayland event handling
        // - Direct /dev/input reading for headless
        // For now, this is a stub
    }
}