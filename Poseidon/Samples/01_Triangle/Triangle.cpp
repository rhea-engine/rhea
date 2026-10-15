/* Copyright (c) 2019-2026 Meworla. MIT License; see LICENSE in the project root. */
#include "../SampleScene.h"

using namespace poseidon::samples;

class TriangleSample : public PoseidonApp {
public:
    void setup() override {
        m_scene.init(getCore(), getContext(), createSampleShader("triangle", false));
        const SampleVertex vertices[] = {
            {{0, 0.8f, 0}, {1, 0, 0, 1}, {0.5f, 0}},
            {{0.8f, -0.6f, 0}, {0, 1, 0, 1}, {1, 1}},
            {{-0.8f, -0.6f, 0}, {0, 0, 1, 1}, {0, 1}},
        };
        const uint32_t indices[] = {0, 1, 2};
        auto mesh = std::make_unique<poseidon::Mesh>();
        mesh->setViewport(getViewport());
        mesh->setVertices(sizeof(SampleVertex), 3, vertices);
        mesh->setMesh(0, sizeof(uint32_t), 1, indices, m_scene.material.get());
        m_scene.setMesh(std::move(mesh));
    }
    void update(float delta_time) override {
        m_scene.setCamera(math::Vector3(0, 0, -3), static_cast<float>(getWidth()) / getHeight());
    }
private:
    SampleScene m_scene;
};

int main(int argc, char** argv) { return runSample<TriangleSample>(argc, argv, "Poseidon 01: Colored Triangle"); }
