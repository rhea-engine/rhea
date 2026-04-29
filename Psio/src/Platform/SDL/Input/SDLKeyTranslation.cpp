/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "SDLKeyTranslation.h"

namespace psio
{
    Key TranslateSDLScancode(SDL_Scancode sc)
    {
        switch (sc)
        {
            // Letters
        case SDL_SCANCODE_A: return Key::CODE_A;
        case SDL_SCANCODE_B: return Key::CODE_B;
        case SDL_SCANCODE_C: return Key::CODE_C;
        case SDL_SCANCODE_D: return Key::CODE_D;
        case SDL_SCANCODE_E: return Key::CODE_E;
        case SDL_SCANCODE_F: return Key::CODE_F;
        case SDL_SCANCODE_G: return Key::CODE_G;
        case SDL_SCANCODE_H: return Key::CODE_H;
        case SDL_SCANCODE_I: return Key::CODE_I;
        case SDL_SCANCODE_J: return Key::CODE_J;
        case SDL_SCANCODE_K: return Key::CODE_K;
        case SDL_SCANCODE_L: return Key::CODE_L;
        case SDL_SCANCODE_M: return Key::CODE_M;
        case SDL_SCANCODE_N: return Key::CODE_N;
        case SDL_SCANCODE_O: return Key::CODE_O;
        case SDL_SCANCODE_P: return Key::CODE_P;
        case SDL_SCANCODE_Q: return Key::CODE_Q;
        case SDL_SCANCODE_R: return Key::CODE_R;
        case SDL_SCANCODE_S: return Key::CODE_S;
        case SDL_SCANCODE_T: return Key::CODE_T;
        case SDL_SCANCODE_U: return Key::CODE_U;
        case SDL_SCANCODE_V: return Key::CODE_V;
        case SDL_SCANCODE_W: return Key::CODE_W;
        case SDL_SCANCODE_X: return Key::CODE_X;
        case SDL_SCANCODE_Y: return Key::CODE_Y;
        case SDL_SCANCODE_Z: return Key::CODE_Z;

            // Numbers
        case SDL_SCANCODE_1: return Key::CODE_1;
        case SDL_SCANCODE_2: return Key::CODE_2;
        case SDL_SCANCODE_3: return Key::CODE_3;
        case SDL_SCANCODE_4: return Key::CODE_4;
        case SDL_SCANCODE_5: return Key::CODE_5;
        case SDL_SCANCODE_6: return Key::CODE_6;
        case SDL_SCANCODE_7: return Key::CODE_7;
        case SDL_SCANCODE_8: return Key::CODE_8;
        case SDL_SCANCODE_9: return Key::CODE_9;
        case SDL_SCANCODE_0: return Key::CODE_0;

            // Control keys
        case SDL_SCANCODE_RETURN: return Key::CODE_ENTER;
        case SDL_SCANCODE_ESCAPE: return Key::CODE_ESC;
        case SDL_SCANCODE_BACKSPACE: return Key::CODE_BACKSPACE;
        case SDL_SCANCODE_TAB: return Key::CODE_TAB;
        case SDL_SCANCODE_SPACE: return Key::CODE_SPACE;

            // Arrows
        case SDL_SCANCODE_LEFT: return Key::CODE_LEFT;
        case SDL_SCANCODE_RIGHT: return Key::CODE_RIGHT;
        case SDL_SCANCODE_UP: return Key::CODE_UP;
        case SDL_SCANCODE_DOWN: return Key::CODE_DOWN;

            // Modifiers
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_RCTRL:
            return Key::CODE_CTRL;

        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT:
            return Key::CODE_SHIFT;

        case SDL_SCANCODE_LALT:
        case SDL_SCANCODE_RALT:
            return Key::CODE_ALT;

        default:
            return Key::NONE;
        }
    }
}