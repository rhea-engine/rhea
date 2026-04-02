#include "Win32Backend.h"

#include <Psio/Input/Input.h>
#include <Psio/Window/Window.h>
#include <Psio/Timer.h>
#include <Psio/Input/Keyboard.h>
#include <Psio/IPC/IO.h>
#include <Psio/Fs/Fs.h>

// KRITISCH: VOR allen Windows-Includes definieren!
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN  // Vermeidet viele veraltete APIs
#endif

// Verhindert automatisches Inkludieren von winsock.h durch windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

// Win32 implementations
#include "Input/InputWin32.h"
#include "Window/Win32Window.h"
#include "Net/NetworkWin32.h"

namespace Psio
{
    Win32Backend::Win32Backend()
    {
    }

    Win32Backend::~Win32Backend() = default;

    const char* Win32Backend::GetName() const
    {
        return "Win32";
    }

    std::unique_ptr<Input> Win32Backend::CreateInput(Engine* engine)
    {
        auto input = std::make_unique<InputWin32>();
        initHwKey();

        return input;
    }

    std::unique_ptr<Window> Win32Backend::CreateNativeWindow(std::string name, int width, int height, WindowOption options)
    {
        return std::make_unique<Win32Window>(std::move(name), width, height);
    }

    std::unique_ptr<Network> Win32Backend::CreateNetwork(Engine* engine)
    {
        return std::make_unique<NetworkWin32>(engine);
    }

    std::unique_ptr<IO> Win32Backend::CreateIO(Engine* engine)
    {
        return std::make_unique<IO>(engine);
    }

    std::unique_ptr<Fs> Win32Backend::CreateFs(Engine* engine)
    {
        return std::make_unique<Fs>("");
    }

    uint64_t Win32Backend::GetTicks()
    {
        return Timer::getTicks(); // or a Win32-specific implementation
    }
}