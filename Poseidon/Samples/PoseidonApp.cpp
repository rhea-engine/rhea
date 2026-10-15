/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "PoseidonApp.h"
#include <Psio/Window/Window.h>
#include <Psio/Input/InputEvent.h>
#include <Psio/Input/Keyboard.h>
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
    // Initialize Poseidon (Renderer)
    m_core = new poseidon::Core();
    m_core->init(m_psio, "auto");
    m_window = m_core->createWindow(title, m_width, m_height, "main");
    m_core->initializeRenderStates();
    
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
        m_psio->iterateInput();
        while (auto evt = m_psio->pollInput())
        {
            if (evt->getId() == psio::InputEventId::WMGR_QUIT)
            {
                m_running = false;
                break;
            }
            
            if (!onInput(*evt))
            {
                // Default: ESC to quit
                if (evt->getId() == psio::InputEventId::KEY_PRESSED && static_cast<psio::KeyInputEvent*>(evt.get())->getKeyCode() == psio::Key::CODE_ESC)
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
        m_core->getFrameRenderer()->advanceTime(deltaTime);
        render();


    }

    return 0;
}

void PoseidonApp::shutdown()
{
    delete m_window;
    m_window = nullptr;
    if (m_core)
    {
        delete m_core;
        m_core = nullptr;
    }
    
    if (m_psio)
    {

        delete m_psio;
        m_psio = nullptr;
    }
    
    LOG(LOG_INFO) << "PoseidonApp shutdown complete";
}

} // namespace poseidon::samples
