#include "UnixBackend.h"

#include <Psio/Input/Input.h>
#include <Psio/Window/Window.h>
#include <Psio/Timer.h>
#include <Psio/Input/Keyboard.h>
#include <Psio/IPC/IO.h>
#include <Psio/Fs/Fs.h>

// Unix implementations
#include "Input/UnixInput.h"
#include "Net/UnixNetwork.h"

namespace psio
{
    UnixBackend::UnixBackend()
    {
    }

    UnixBackend::~UnixBackend() = default;

    const char* UnixBackend::GetName() const
    {
        return "Unix";
    }

    std::unique_ptr<Input> UnixBackend::CreateInput(Engine* engine)
    {
        auto input = std::make_unique<InputUnix>();
        return input;
    }

    std::unique_ptr<Window> UnixBackend::CreateNativeWindow(WindowDesc desc)
    {
        // TODO: Implement Unix window (X11/Wayland) or return nullptr if headless
        // For now, return nullptr - can be implemented later with X11 or SDL
        return nullptr;
    }

    std::unique_ptr<Network> UnixBackend::CreateNetwork(Engine* engine)
    {
        return std::make_unique<NetworkUnix>(engine);
    }

    std::unique_ptr<IO> UnixBackend::CreateIO(Engine* engine)
    {
        return std::make_unique<IO>(engine);
    }

    std::unique_ptr<Fs> UnixBackend::CreateFs(Engine* engine)
    {
        return std::make_unique<Fs>("");
    }

    uint64_t UnixBackend::GetTicks()
    {
        return Timer::getTicks();
    }
}