#include "SDLInput.h"

#include "SDLKeyTranslation.h"

#include <Psio/Input/InputEvent.h>
#include <Psio/Window/Window.h>

#include <SDL3/SDL.h>
#include <memory>
#include "../../../Input/InputEventBuilder.h"

namespace psio
{
    SDLInput::SDLInput(Engine* engine)
        : m_engine(engine)
    {
    }

    void SDLInput::update()
    {
        SDL_Event evt;
        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_EVENT_QUIT:
            {
                addEvent(BuildQuitEvent());
                break;
            }
            case SDL_EVENT_KEY_DOWN:
            {
                addEvent(BuildKeyEvent(
                    InputEventId::KEY_PRESSED,
                    static_cast<uint32_t>(TranslateSDLScancode(evt.key.scancode)),
                    static_cast<uint32_t>(evt.key.scancode),
                    0,
                    (evt.key.mod & SDL_KMOD_SHIFT) != 0,
                    (evt.key.mod & SDL_KMOD_ALT) != 0,
                    (evt.key.mod & SDL_KMOD_CTRL) != 0));
                break;
            }
            case SDL_EVENT_KEY_UP:
            {
                addEvent(BuildKeyEvent(
                    InputEventId::KEY_RELEASED,
                    static_cast<uint32_t>(TranslateSDLScancode(evt.key.scancode)),
                    static_cast<uint32_t>(evt.key.scancode),
                    0,
                    (evt.key.mod & SDL_KMOD_SHIFT) != 0,
                    (evt.key.mod & SDL_KMOD_ALT) != 0,
                    (evt.key.mod & SDL_KMOD_CTRL) != 0));
                break;
            }
            case SDL_EVENT_TEXT_INPUT:
            {
                const char* txt = evt.text.text;
                SDL_Keymod mods = SDL_GetModState();
                addEvent(BuildTextEvent(					
                    txt ? std::string(txt) : std::string(),
                    (mods & SDL_KMOD_SHIFT) != 0,
                    (mods & SDL_KMOD_ALT) != 0,
                    (mods & SDL_KMOD_CTRL) != 0));
                break;            
            }
            case SDL_EVENT_MOUSE_MOTION:
            {
                addEvent(BuildMouseEvent(
                    InputEventId::MOUSE_MOVE,
                    evt.motion.x,
                    evt.motion.y,
                    (evt.motion.state & SDL_BUTTON_LMASK) != 0,
                    (evt.motion.state & SDL_BUTTON_RMASK) != 0,
                    (evt.motion.state & SDL_BUTTON_MMASK) != 0,
                    false,
                    false,
                    false));
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                const auto id = (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                    ? InputEventId::MOUSE_BTN_PRESSED
                    : InputEventId::MOUSE_BTN_RELEASED;

                addEvent(BuildMouseEvent(
                    id,
                    evt.button.x,
                    evt.button.y,
                    evt.button.button == SDL_BUTTON_LEFT,
                    evt.button.button == SDL_BUTTON_RIGHT,
                    evt.button.button == SDL_BUTTON_MIDDLE,
                    false,
                    false,
					false));
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                SDL_Window* sdlWindow = SDL_GetWindowFromID(evt.window.windowID);
                if (!sdlWindow)
                    break;

                SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
                if (!props)
                    break;

                auto* win = static_cast<psio::Window*>(
                    SDL_GetPointerProperty(props, "psio.window", nullptr));

                if (!win)
                    break;

                int pixelW = 0;
                int pixelH = 0;
                if (!SDL_GetWindowSizeInPixels(sdlWindow, &pixelW, &pixelH))
                    break;

                addEvent(std::make_shared<WinSizeEvent>(
                    InputEventId::WIN_SIZE,
                    pixelW,
                    pixelH,
                    win));

                break;
            }
            default:
                break;
            }
        }
    }
}