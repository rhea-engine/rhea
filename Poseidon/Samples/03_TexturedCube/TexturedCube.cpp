/* Copyright (c) 2019-2026 Meworla. MIT License; see LICENSE in the project root. */
#include "../SampleScene.h"

using namespace poseidon::samples;

class TexturedCubeSample : public PoseidonApp {
public:
    void setup() override {
        m_scene.init(getCore(), getContext(), createSampleShader("textured_cube", true));
        m_scene.texture.reset(getCore()->createTexture(0, 1));
        // Generate an RGBA checkerboard so the sample needs no external images.
        constexpr unsigned int size = 64;
        std::array<unsigned char, size * size * 4> pixels{};
        for (unsigned int y = 0; y < size; ++y) {
            for (unsigned int x = 0; x < size; ++x) {
                const bool light = ((x / 8) + (y / 8)) % 2 == 0;
                const size_t offset = (y * size + x) * 4;
                pixels[offset] = light ? 240 : 35;
                pixels[offset + 1] = light ? 210 : 90;
                pixels[offset + 2] = light ? 100 : 170;
                pixels[offset + 3] = 255;
            }
        }
        if (!m_scene.texture->load(0, size, size, pixels.data()))
            throw std::runtime_error("Cannot upload the checkerboard texture.");
        m_scene.material->addTexture(m_scene.texture.get());
        m_scene.setMesh(createCubeMesh(getViewport(), m_scene.material.get(), false));
    }
    void update(float delta_time) override {
        m_rotation += delta_time * 0.5f;
        const math::Quaternion rotation(m_rotation, math::Vector3::UNIT_Y);
        m_scene.setTransform(math::Matrix4::compose(math::Vector3::ZERO, rotation, math::Vector3::UNIT_SCALE));
        m_scene.setCamera(math::Vector3(0, 2, -6), static_cast<float>(getWidth()) / getHeight());
    }
private:
    SampleScene m_scene;
    float m_rotation = 0;
};

int main(int argc, char** argv) { return runSample<TexturedCubeSample>(argc, argv, "Poseidon 03: Textured Cube"); }
