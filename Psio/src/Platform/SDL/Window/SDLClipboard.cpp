#include "SDLClipboard.h"
#include <SDL3/SDL.h>
#include <string>
#include <string_view>

namespace psio
{
    bool SDLClipboard::setText(std::string_view text)
    {
        // SDL expects a null-terminated string.
        const std::string terminated(text);
        return SDL_SetClipboardText(terminated.c_str());
    }

    bool SDLClipboard::getText(std::string& text)
    {
        text.clear();

        if (!SDL_HasClipboardText())
            return false;

        char* clipboardText = SDL_GetClipboardText();
        if (!clipboardText)
            return false;

        text = clipboardText;
        SDL_free(clipboardText);
        return true;
    }
}