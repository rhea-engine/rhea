/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "../PoseidonApp.h"
#include <Poseidon/Mesh.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Poseidon/Texture.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Viewport.h>
#include <Poseidon/FrameRenderer.h>
#include <Hippios/VertexBuffer.h>
#include <Psio/Log.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

using namespace poseidon::samples;

/**
 * @brief Textured Cube Sample
 * 
 * Zeigt:
 * - Texture Loading
 * - UV-Mapping
 * - Material mit Textur
 */
class TexturedCubeSample : public PoseidonApp
{
private:
    poseidon::Entity* m_cube = nullptr;
    poseidon::Camera* m_camera = nullptr;
    float m_rotation = 0.0f;

public:
    void setup() override
    {
        LOG(LOG_INFO) << "Setting up Textured Cube sample...";
        
        auto* viewport = getCore()->getFrameRenderer()->getViewport();

        // Create shader
        poseidon::Shader* shader = getCore()->createShader("uvshader");
        if (!shader->loadFromFile("rhea/shaders/uvshader.hlsl", "VShader", "PShader"))
        {
            LOG(LOG_ERROR) << "Failed to load shader!";
            m_running = false;
            return;
        }

        // Load texture
        poseidon::Texture* texture = getCore()->createTexture("Samples/assets/images/kies.png");
        if (!texture)
        {
            LOG(LOG_ERROR) << "Failed to load texture!";
            m_running = false;
            return;
        }

        // Create material with texture
        poseidon::Material* material = getCore()->createMaterial();
        material->setShader(shader);
        material->addTexture(texture);

        // Create cube mesh
        poseidon::Mesh* mesh = createCubeMesh();
        mesh->setViewport(viewport);

        // Create entity
        m_cube = getCore()->createEntity(mesh);
        m_cube->setMaterial(material);
        m_cube->get_position()->setPosition(0.0f, 0.0f, 4.0f);
        viewport->addRenderChild(m_cube);

        // Create camera
        m_camera = getCore()->createCamera();
        m_camera->get_position()->setPosition(0.0f, 3.0f, -8.0f);
        m_camera->get_position()->lookAt(
            math::Vector3(0.0f, 0.0f, 0.0f), 
            math::Vector3::UNIT_Z
        );
        m_camera->setAspectRatio((float)getWidth() / (float)getHeight());
        
        viewport->addChild(m_camera);
        viewport->setCamera(m_camera);
        
        LOG(LOG_INFO) << "Textured Cube setup complete";
    }

    void update(float deltaTime) override
    {
        // Rotate cube
        m_rotation += deltaTime * 0.5f;
        if (m_rotation > 6.28f)
            m_rotation -= 6.28f;

		auto pos = m_cube->getWorldPosition();
        pos->setWorldPosition(math::Vector3(0, 0, 0));
        pos->setOrientation(Quaternion(m_rotation, math::Vector3(0.0f, 1.0f, 0.0f)));
        pos->moveRelative(math::Vector3(4.0f, 0.0f, 0.0f));
    }

    void render() override
    {
        getCore()->iterate();
    }

private:
    poseidon::Mesh* createCubeMesh()
    {
        // Same as Sample 02
        const int num_vertices = 24;
        const int num_faces = 12;

        hippios::vertex_uv vertices[num_vertices] = {
            // Front Face
            {-1.0f, -1.0f, -1.0f, 0.0f, 1.0f}, {-1.0f,  1.0f, -1.0f, 0.0f, 0.0f},
            { 1.0f,  1.0f, -1.0f, 1.0f, 0.0f}, { 1.0f, -1.0f, -1.0f, 1.0f, 1.0f},
            {-1.0f, -1.0f,  1.0f, 1.0f, 1.0f}, { 1.0f, -1.0f,  1.0f, 0.0f, 1.0f},
            { 1.0f,  1.0f,  1.0f, 0.0f, 0.0f}, {-1.0f,  1.0f,  1.0f, 1.0f, 0.0f},
            {-1.0f,  1.0f, -1.0f, 0.0f, 1.0f}, {-1.0f,  1.0f,  1.0f, 0.0f, 0.0f},
            { 1.0f,  1.0f,  1.0f, 1.0f, 0.0f}, { 1.0f,  1.0f, -1.0f, 1.0f, 1.0f},
            {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f}, { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f},
            { 1.0f, -1.0f,  1.0f, 0.0f, 0.0f}, {-1.0f, -1.0f,  1.0f, 1.0f, 0.0f},
            {-1.0f, -1.0f,  1.0f, 0.0f, 1.0f}, {-1.0f,  1.0f,  1.0f, 0.0f, 0.0f},
            {-1.0f,  1.0f, -1.0f, 1.0f, 0.0f}, {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f},
            { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f}, { 1.0f,  1.0f, -1.0f, 0.0f, 0.0f},
            { 1.0f,  1.0f,  1.0f, 1.0f, 0.0f}, { 1.0f, -1.0f,  1.0f, 1.0f, 1.0f}
        };

        unsigned long indices[] = {
            0,1,2, 0,2,3,    4,5,6, 4,6,7,    8,9,10, 8,10,11,
            12,13,14, 12,14,15,  16,17,18, 16,18,19,  20,21,22, 20,22,23
        };

        poseidon::Mesh* mesh = new poseidon::Mesh();
        mesh->setVertices(sizeof(hippios::vertex_uv), num_vertices, vertices);
        mesh->setIndices(sizeof(unsigned long), num_faces, indices);
        
        return mesh;
    }
};

int main()
{
    TexturedCubeSample app;
    app.init("Poseidon Sample 03: Textured Cube");
    return app.run();
}