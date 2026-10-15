/* Copyright (c) 2019-2026 Meworla. MIT License; see LICENSE in the project root. */
#pragma once
#include <Psio/Psio.h>
#include <Psio/Input/InputEvent.h>
#include <Psio/Input/Keyboard.h>
#include <Poseidon/Core.h>
#include <Poseidon/Window.h>
#include <Poseidon/RenderContext.h>
#include <Poseidon/Shader.h>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <iostream>

namespace poseidon::samples {
struct SampleVertex { float position[3]; float color[4]; float uv[2]; };

// Samples own their scene resources; this class owns the window and renderer.
class PoseidonApp {
public:
    virtual ~PoseidonApp();
    void init(const char* title, const std::string& backend, const std::filesystem::path& asset_directory);
    int run(unsigned int frame_limit = 0);
    virtual void setup() = 0;
    virtual void update(float delta_time) {}
    virtual bool onInput(const psio::InputEvent& event) { return false; }
protected:
    Core* getCore() { return m_core.get(); }
    Viewport* getViewport();
    RenderContext* getContext();
    Shader* createSampleShader(const std::string& name, bool textured);
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
private:
    std::unique_ptr<psio::Engine> m_psio;
    std::unique_ptr<Core> m_core;
    std::unique_ptr<Window> m_window;
    std::filesystem::path m_asset_directory;
    int m_width = 1152;
    int m_height = 864;
};

template<class Sample>
int runSample(int argc, char** argv, const char* title) {
    try {
        std::string backend = "auto";
        unsigned int frame_limit = 0;
        auto asset_directory = std::filesystem::absolute(argv[0]).parent_path() / "Data" / "PoseidonSamples";
        for (int index = 1; index < argc; ++index) {
            const std::string argument = argv[index];
            if (argument == "--backend" && index + 1 < argc) {
                backend = argv[++index];
                if (backend != "auto" && backend != "opengl" && backend != "d3d11")
                    throw std::runtime_error("Backend must be auto, opengl, or d3d11.");
            } else if (argument == "--frames" && index + 1 < argc) {
                const std::string count = argv[++index];
                size_t parsed = 0;
                const auto value = std::stoul(count, &parsed);
                if (parsed != count.size() || count.find('-') != std::string::npos || value > 1000000)
                    throw std::runtime_error("Frame count must be between 0 and 1000000.");
                frame_limit = static_cast<unsigned int>(value);
            } else if (argument == "--assets" && index + 1 < argc) {
                asset_directory = argv[++index];
            } else {
                throw std::runtime_error("Usage: sample [--backend auto|opengl|d3d11] [--frames count] [--assets directory]");
            }
        }
        Sample sample;
        sample.init(title, backend, asset_directory);
        return sample.run(frame_limit);
    } catch (const std::exception& error) {
        std::cerr << "Sample failed: " << error.what() << '\n';
        return 1;
    }
}
}
