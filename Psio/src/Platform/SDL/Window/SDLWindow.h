/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_SDLWINDOW_H
#define PSIO_SDLWINDOW_H

#include <Psio/Psio.h>
#include <Psio/Window/Window.h>
#include <string>

struct SDL_Window;

namespace psio
{
    class SDLWindow final : public Window
    {
    public:
        SDLWindow(WindowDesc desc);
        ~SDLWindow() override;

        void create() override;
        void destroy() override;
        void* getHandle() override;
        void SetFullscreen(bool mode) override;

    private:
        std::string m_name;
        SDL_Window* m_window = nullptr;
        WindowDesc desc{};
    };
}
#endif // !PSIO_SDLWINDOW_H
