/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "PoseidonApp.h"
#include <Psio/Window.h>
#include <Poseidon/FrameRenderer.h>
#include <Psio/Log.h>

namespace poseidon::samples {

PoseidonApp::~PoseidonApp()
{
    shutdown();
}

void PoseidonApp::init(const char* title, int width, int height)
{
    m_width = width;
    m_height = height;

    // Initialize Psio (Window + Input)
    m_psio = new psio::Engine();
    m_psio->init();

    // Create window
    psio::WindowDesc desc;
    desc.title = title;
    desc.width = m_width;
    desc.height = m_height;
    desc.fullscreen = false;
    m_psio->createWindow(desc);

    // Initialize Hippios (Graphics Backend: D3D11/OpenGL)
    m_hippios = new hippios::Engine();
    m_hippios->init(m_psio->getWindow());

    // Initialize Poseidon (Renderer)
    m_core = new poseidon::Core();
    m_core->init(m_hippios);
    
    LOG(LOG_INFO) << "PoseidonApp initialized: " << title;
}

int PoseidonApp::run()
{
    setup();

    uint64_t lastTicks = m_psio->getTicks();

    while (m_running)
    {
        // Calculate delta time
        uint64_t currentTicks = m_psio->getTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000000.0f;
        lastTicks = currentTicks;

        // Handle input
        psio::InputEvent evt;
        while (m_psio->pollEvent(evt))
        {
            if (evt.isQuit())
            {
                m_running = false;
                break;
            }
            
            if (!onInput(evt))
            {
                // Default: ESC to quit
                if (evt.isKeyPressed(psio::Key::CODE_ESCAPE))
                {
                    LOG(LOG_INFO) << "ESC pressed - exiting";
                    m_running = false;
                }
            }
        }

        if (!m_running)
            break;

        // Update
        update(deltaTime);

        // Render
        m_core->beginFrame();
        render();
        m_core->endFrame();
        m_hippios->present();
    }

    return 0;
}

void PoseidonApp::shutdown()
{
    if (m_core)
    {
        delete m_core;
        m_core = nullptr;
    }
    
    if (m_hippios)
    {
        m_hippios->shutdown();
        delete m_hippios;
        m_hippios = nullptr;
    }
    
    if (m_psio)
    {
        m_psio->shutdown();
        delete m_psio;
        m_psio = nullptr;
    }
    
    LOG(LOG_INFO) << "PoseidonApp shutdown complete";
}

} // namespace poseidon::samples