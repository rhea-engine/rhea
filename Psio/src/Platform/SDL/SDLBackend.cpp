#include "SDLBackend.h"

#include "Input/SDLInput.h"
#include "Window/SDLWindow.h"

#include <Psio/Net/Network.h>
#include <Psio/IPC/IO.h>
#include <Psio/Fs/Fs.h>
#include "Net/NullNetwork.h"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <utility>

namespace psio
{
    SDLBackend::SDLBackend()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    SDLBackend::~SDLBackend()
    {
        SDL_Quit();
    }

    const char* SDLBackend::GetName() const
    {
        return "SDL";
    }

    std::unique_ptr<Input> SDLBackend::CreateInput(Engine* engine)
    {
        return std::make_unique<SDLInput>(engine);
    }

    std::unique_ptr<Window> SDLBackend::CreateNativeWindow(WindowDesc desc)
    {
        return std::make_unique<SDLWindow>(desc);
    }

    std::unique_ptr<Network> SDLBackend::CreateNetwork(Engine* engine)
    {
        return std::make_unique<NullNetwork>(engine);
    }

    std::unique_ptr<IO> SDLBackend::CreateIO(Engine* engine)
    {
        return std::make_unique<IO>(engine);
    }

    std::unique_ptr<Fs> SDLBackend::CreateFs(Engine* engine)
    {
        return std::make_unique<Fs>("");
    }

    uint64_t SDLBackend::GetTicks()
    {
        return SDL_GetTicksNS() / 1000;
    }
}