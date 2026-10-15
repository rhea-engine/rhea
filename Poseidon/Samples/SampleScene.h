/* Copyright (c) 2019-2026 Meworla. MIT License; see LICENSE in the project root. */
#pragma once
#include "PoseidonApp.h"
#include <Poseidon/Mesh.h>
#include <Poseidon/Material.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Renderable.h>
#include <Poseidon/RenderWorld.h>
#include <cstdint>
#include <array>

namespace poseidon::samples {
// Own GPU resources until the sample is destroyed, before PoseidonApp shuts down.
struct SampleScene {
    Core* core = nullptr;
    RenderContext* context = nullptr;
    RenderableHandle renderable = InvalidRenderable;
    CameraHandle camera = InvalidCamera;
    RenderNodeId node = InvalidRenderNode;
    std::unique_ptr<Texture> texture;
    std::unique_ptr<Material> material;
    std::unique_ptr<Mesh> mesh;

    SampleScene() = default;
    SampleScene(const SampleScene&) = delete;
    SampleScene& operator=(const SampleScene&) = delete;
    ~SampleScene() {
        if (!core)
            return;
        if (node != InvalidRenderNode)
            context->unregisterOrderNode(node);
        if (renderable != InvalidRenderable)
            core->getRenderWorld()->destroyRenderable(renderable);
        if (camera != InvalidCamera) {
            context->setCameraHandle(InvalidCamera);
            core->getRenderWorld()->destroyCamera(camera);
        }
    }

    void init(Core* graphics_core, RenderContext* render_context, Shader* shader) {
        core = graphics_core;
        context = render_context;
        material.reset(core->createMaterial());
        material->setShader(shader);
        camera = core->getRenderWorld()->createCamera();
        context->setCameraHandle(camera);
    }

    void setMesh(std::unique_ptr<Mesh> sample_mesh) {
        mesh = std::move(sample_mesh);
        renderable = core->getRenderWorld()->createRenderable(mesh.get());
        node = context->registerOrderNode(1, InvalidRenderNode);
        context->attachRenderable(node, renderable);
        setTransform(math::Matrix4::IDENTITY);
    }

    void setTransform(const math::Matrix4& transform) {
        auto* object = core->getRenderWorld()->resolveRenderable(renderable);
        object->worldTransform = transform;
        object->inverseTranspose = transform.inverse().transpose();
    }

    void setCamera(const math::Vector3& position, float aspect_ratio) {
        auto* view = core->getRenderWorld()->resolveCamera(camera);
        view->setPosition(position);
        const auto forward = (math::Vector3::ZERO - position).normalizedCopy();
        const auto right = math::Vector3::UNIT_Y.crossProduct(forward).normalizedCopy();
        const auto up = forward.crossProduct(right);
        view->setOrientation(math::Quaternion(right, up, forward));
        view->setAspectRatio(aspect_ratio);
    }
};

inline std::unique_ptr<Mesh> createCubeMesh(Viewport* viewport, Material* material, bool colored) {
    const float positions[6][4][3] = {
        {{-1,-1,-1},{-1,1,-1},{1,1,-1},{1,-1,-1}},
        {{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}},
        {{-1,1,-1},{-1,1,1},{1,1,1},{1,1,-1}},
        {{-1,-1,-1},{1,-1,-1},{1,-1,1},{-1,-1,1}},
        {{-1,-1,1},{-1,1,1},{-1,1,-1},{-1,-1,-1}},
        {{1,-1,-1},{1,1,-1},{1,1,1},{1,-1,1}},
    };
    const float colors[6][4] = {
        {1,0.25f,0.2f,1}, {0.2f,0.8f,1,1}, {0.3f,1,0.4f,1},
        {1,0.8f,0.2f,1}, {0.7f,0.3f,1,1}, {1,0.4f,0.7f,1},
    };
    const float uv[4][2] = {{0,1},{0,0},{1,0},{1,1}};
    std::array<SampleVertex, 24> vertices{};
    std::array<uint32_t, 36> indices{};
    for (size_t face = 0; face < 6; ++face) {
        for (size_t corner = 0; corner < 4; ++corner) {
            auto& vertex = vertices[face * 4 + corner];
            for (size_t axis = 0; axis < 3; ++axis)
                vertex.position[axis] = positions[face][corner][axis];
            for (size_t channel = 0; channel < 4; ++channel)
                vertex.color[channel] = colored ? colors[face][channel] : 1.0f;
            vertex.uv[0] = uv[corner][0];
            vertex.uv[1] = uv[corner][1];
        }
        const uint32_t start = static_cast<uint32_t>(face * 4);
        const uint32_t face_indices[] = {start, start + 1, start + 2, start, start + 2, start + 3};
        for (size_t index = 0; index < 6; ++index)
            indices[face * 6 + index] = face_indices[index];
    }
    auto mesh = std::make_unique<Mesh>();
    mesh->setViewport(viewport);
    mesh->setVertices(sizeof(SampleVertex), vertices.size(), vertices.data());
    mesh->setMesh(0, sizeof(uint32_t), 12, indices.data(), material);
    return mesh;
}
}
