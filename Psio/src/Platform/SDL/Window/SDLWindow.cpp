#include "SDLWindow.h"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <utility>
#include <Psio/Log.h>

namespace Psio
{
    SDLWindow::SDLWindow(std::string name, int width, int height, WindowOption options)
        : Window(width, height),
        m_name(std::move(name)),
        m_options(options)
    {
    }

    SDLWindow::~SDLWindow()
    {
        destroy();
    }

    void SDLWindow::create()
    {
        if (m_window)
            return;

        Uint32 windowFlags = SDL_WINDOW_RESIZABLE;
        if ((m_options & WindowOption::Fullscreen) == WindowOption::Fullscreen)
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }
        if ((m_options & WindowOption::Vulkan) == WindowOption::Vulkan)
        {
            windowFlags |= SDL_WINDOW_VULKAN;
		}
        if (hasFlag(m_options, WindowOption::OpenGL))
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef _DEBUG
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

            windowFlags |= SDL_WINDOW_OPENGL;
        }
        m_window = SDL_CreateWindow(
            m_name.c_str(),
            m_width,
            m_height,
            windowFlags);

        if (!m_window)
        {
            throw std::runtime_error(SDL_GetError());
        }

        // Store the Psio window wrapper on the SDL window.
        SDL_PropertiesID props = SDL_GetWindowProperties(m_window);
        if (!props) {
            throw std::runtime_error(SDL_GetError());
        }

        if (!SDL_SetPointerProperty(props, "psio.window", this)) {
            throw std::runtime_error(SDL_GetError());
        }

        if (!SDL_StartTextInput(m_window)) {
            LOG(LOG_ERROR) << "SDL_StartTextInput failed: " << SDL_GetError();
        }
    }

    void SDLWindow::destroy()
    {
        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
    }

    void* SDLWindow::getHandle()
    {
        return m_window;
    }

    void SDLWindow::SetFullscreen(bool mode)
    {
        if (!m_window)
            return;

        SDL_SetWindowFullscreen(m_window, mode);
    }
}