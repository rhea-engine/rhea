#pragma once
#include <Psio/Psio.h>
#include <Poseidon/Core.h>
#include <memory>

namespace poseidon::samples {

/**
 * @brief Minimale Basisklasse für Poseidon Low-Level Samples
 * 
 * Zeigt, wie man Poseidon Renderer direkt nutzt ohne Rhea Engine.
 * Enthält nur: Psio (Window+Input) + Hippios (Graphics Backend) + Poseidon (Renderer)
 */
class PoseidonApp
{
protected:
    psio::Engine* m_psio = nullptr;
    poseidon::Core* m_core = nullptr;
    
    int m_width = 1152;
    int m_height = 864;
    bool m_running = true;

public:
    PoseidonApp() = default;
    virtual ~PoseidonApp();

    // Lifecycle
    void init(const char* title = "Poseidon Sample", int width = 1152, int height = 864);
    int run();
    void shutdown();

    // Override these in your sample
    virtual void setup() {}
    virtual void update(float deltaTime) {}
    virtual void render() {}
    virtual bool onInput(const psio::InputEvent& evt) { return false; }

protected:
    poseidon::Core* getCore() { return m_core; }
    psio::Engine* getPsio() { return m_psio; }
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
};

} // namespace poseidon::samples