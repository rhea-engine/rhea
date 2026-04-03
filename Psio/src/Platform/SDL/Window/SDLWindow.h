#ifndef PSIO_SDLWINDOW_H
#define PSIO_SDLWINDOW_H

#include <Psio/Psio.h>
#include <Psio/Window/Window.h>
#include <string>

struct SDL_Window;

namespace Psio
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
