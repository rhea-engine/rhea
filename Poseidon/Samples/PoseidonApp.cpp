/* Copyright (c) 2019-2026 Meworla. MIT License; see LICENSE in the project root. */
#include "PoseidonApp.h"
#include <Poseidon/FrameRenderer.h>
#include <Poseidon/Window.h>
#include <Poseidon/Viewport.h>
#include <Poseidon/RenderWorld.h>
#include <Psio/Window/Window.h>
#include <Psio/Log.h>
#include <cstddef>
#include <fstream>
#include <iterator>

namespace poseidon::samples {
namespace {
std::string readShader(const std::filesystem::path& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream)
        throw std::runtime_error("Cannot read sample shader: " + path.string());
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}
}
PoseidonApp::~PoseidonApp() = default;

void PoseidonApp::init(const char* title, const std::string& backend, const std::filesystem::path& asset_directory) {
    m_asset_directory = asset_directory;
    m_psio = std::make_unique<psio::Engine>();
    m_core = std::make_unique<Core>();
    m_core->init(m_psio.get(), backend);
    if (!m_core->get_renderer())
        throw std::runtime_error("Requested renderer is unavailable.");
    m_window.reset(m_core->createWindow(title, m_width, m_height, "main"));
    if (!m_window->getNativeWindow() || !m_window->getSwapChain() || !m_window->getRenderTarget())
        throw std::runtime_error("Cannot create the sample window or render target.");
    m_core->initializeRenderStates();
    LOG(LOG_INFO) << "Poseidon sample backend: " << m_core->getRendererBackend();
}
Viewport* PoseidonApp::getViewport() { return m_window->getDefaultViewport(); }
RenderContext* PoseidonApp::getContext() {
    std::string name = "default";
    return getViewport()->getRenderContext(name);
}
Shader* PoseidonApp::createSampleShader(const std::string& name, bool textured) {
    const bool opengl = m_core->getRendererBackend() == "opengl";
    auto vertex_source = readShader(m_asset_directory / (opengl ? "sample_vs.glsl" : "sample.hlsl"));
    auto pixel_source = readShader(m_asset_directory / (opengl ? (textured ? "texture_fs.glsl" : "color_fs.glsl") : "sample.hlsl"));
    ShaderProgramSourceDesc description;
    description.vertexSource = reinterpret_cast<const unsigned char*>(vertex_source.data());
    description.vertexSourceLength = vertex_source.size();
    description.pixelSource = reinterpret_cast<const unsigned char*>(pixel_source.data());
    description.pixelSourceLength = pixel_source.size();
    description.vertexEntry = "VShader";
    description.pixelEntry = textured ? "PTexture" : "PColor";
    if (textured)
        description.sampler_bindings.push_back({"sample_texture", 0});
    auto* shader = m_core->createShader(name);
    if (!shader->loadFromSourceDesc(description))
        throw std::runtime_error("Cannot compile sample shader: " + name);
    using Format = hippios::InputLayoutFormat;
    shader->setInputLayout({
        {"POSITION", 0, 0, Format::R32G32B32_FLOAT, 0, offsetof(SampleVertex, position)},
        {"COLOR", 0, 1, Format::R32G32B32A32_FLOAT, 0, offsetof(SampleVertex, color)},
        {"TEXCOORD", 0, 2, Format::R32G32_FLOAT, 0, offsetof(SampleVertex, uv)},
    }, sizeof(SampleVertex));
    shader->setCullMode(false);
    shader->setSamplerState(m_core->getSamplerState("linear"));
    shader->finalize();
    return shader;
}
int PoseidonApp::run(unsigned int frame_limit) {
    setup();
    auto last_ticks = m_psio->getTicks();
    unsigned int frames = 0;
    bool running = true;
    while (running) {
        const auto current_ticks = m_psio->getTicks();
        const float delta_time = static_cast<float>(current_ticks - last_ticks) / 1000000.0f;
        last_ticks = current_ticks;
        m_psio->iterateInput();
        while (auto event = m_psio->pollInput()) {
            if (event->getId() == psio::InputEventId::WMGR_QUIT) {
                running = false;
                break;
            }
            if (event->getId() == psio::InputEventId::WIN_SIZE) {
                auto* resize_event = static_cast<psio::WinSizeEvent*>(event.get());
                if (resize_event->getWindow() == m_window->getNativeWindow() && resize_event->width() && resize_event->height()) {
                    m_width = resize_event->width();
                    m_height = resize_event->height();
                    m_window->onResize(m_width, m_height);
                }
            }
            if (!onInput(*event) && event->getId() == psio::InputEventId::KEY_PRESSED &&
                static_cast<psio::KeyInputEvent*>(event.get())->getKeyCode() == psio::Key::CODE_ESC)
                running = false;
        }
        if (!running)
            break;
        update(delta_time);
        m_core->getFrameRenderer()->advanceTime(delta_time);
        if (m_core->iterate())
            throw std::runtime_error("Renderer could not complete a frame.");
        ++frames;
        if (frame_limit && frames >= frame_limit)
            break;
    }
    LOG(LOG_INFO) << "Poseidon sample completed " << frames << " frames";
    return 0;
}
}
