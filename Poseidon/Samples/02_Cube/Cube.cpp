/* Copyright (c) 2019-2026 Meworla. MIT License; see LICENSE in the project root. */
#include "../SampleScene.h"

using namespace poseidon::samples;

class CubeSample : public PoseidonApp {
public:
    void setup() override {
        m_scene.init(getCore(), getContext(), createSampleShader("colored_cube", false));
        m_scene.setMesh(createCubeMesh(getViewport(), m_scene.material.get(), true));
    }
    void update(float delta_time) override {
        m_rotation += delta_time * 0.5f;
        const math::Quaternion rotation(m_rotation, math::Vector3::UNIT_Y);
        m_scene.setTransform(math::Matrix4::compose(math::Vector3::ZERO, rotation, math::Vector3::UNIT_SCALE));
        m_scene.setCamera(m_camera_position, static_cast<float>(getWidth()) / getHeight());
    }
    bool onInput(const psio::InputEvent& event) override {
        if (event.getId() != psio::InputEventId::KEY_PRESSED)
            return false;
        const auto key = static_cast<const psio::KeyInputEvent&>(event).getKeyCode();
        switch (key) {
        case psio::Key::CODE_LEFT: m_camera_position.x -= 0.2f; break;
        case psio::Key::CODE_RIGHT: m_camera_position.x += 0.2f; break;
        case psio::Key::CODE_UP: m_camera_position.z += 0.2f; break;
        case psio::Key::CODE_DOWN: m_camera_position.z -= 0.2f; break;
        case psio::Key::CODE_HOME: m_camera_position.y += 0.2f; break;
        case psio::Key::CODE_END: m_camera_position.y -= 0.2f; break;
        default: return false;
        }
        // Keep the camera away from the origin and the look-at pole.
        if (m_camera_position.z > -2.5f)
            m_camera_position.z = -2.5f;
        return true;
    }
private:
    SampleScene m_scene;
    math::Vector3 m_camera_position{0, 2, -6};
    float m_rotation = 0;
};

int main(int argc, char** argv) { return runSample<CubeSample>(argc, argv, "Poseidon 02: Rotating Cube"); }
