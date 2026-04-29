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
#include <Poseidon/Camera.h>
#include <Poseidon/RenderContext.h>
#include <Poseidon/RenderWorld.h>
#include <Poseidon/FrameRenderer.h>
#include <Poseidon/Renderable.h>
#include <Hippios/VertexBuffer.h>
#include <Psio/Log.h>

using namespace poseidon::samples;

/**
 * @brief Rotating 3D Cube Sample
 *
 * Zeigt:
 * - 3D Mesh mit Indices
 * - Camera Setup
 * - RenderContext und Renderable
 * - Transform Updates
 * - Animation (Rotation)
 * - Camera-Steuerung (Arrow Keys)
 */
class CubeSample : public PoseidonApp
{
private:
    poseidon::RenderContext* m_context = nullptr;
    poseidon::RenderableHandle m_renderable = poseidon::InvalidRenderable;
    poseidon::CameraHandle m_camera = poseidon::InvalidCamera;
    float m_rotation = 0.0f;
    math::Vector3 m_cameraPos;

public:
    void setup() override
    {
        LOG(LOG_INFO) << "Setting up Cube sample...";
        
        auto* renderWorld = getCore()->getRenderWorld();

        // Create render context
        m_context = new poseidon::RenderContext();
        
        // Create shader
        poseidon::Shader* shader = getCore()->createShader("uvshader");
        if (!shader->loadFromFile("rhea/shaders/uvshader_vp.hlsl", "VShader", "PShader"))
        {
            LOG(LOG_ERROR) << "Failed to load shader!";
            m_running = false;
            return;
        }

        // Create material
        poseidon::Material* material = getCore()->createMaterial();
        material->setShader(shader);

        // Create cube mesh
        poseidon::Mesh* mesh = createCubeMesh(material);

        // Create Renderable
        m_renderable = renderWorld->createRenderable();
        poseidon::Renderable* renderable = renderWorld->resolveRenderable(m_renderable);
        if (renderable)
        {
            renderable->mesh = mesh;
            renderable->visible = true;
            
            // Set initial transform
            renderable->worldTransform = math::Matrix4::IDENTITY;
            renderable->worldTransform.setTranslation(math::Vector3(0.0f, 0.0f, 4.0f));
        }

        // Add to context
        poseidon::RenderNodeId rootNode = m_context->getRootNode();
        m_context->attachRenderable(rootNode, m_renderable);

        // Create camera
        m_camera = renderWorld->createCamera();
        poseidon::Camera* camera = renderWorld->resolveCamera(m_camera);
        if (camera)
        {
            m_cameraPos = math::Vector3(0.0f, 3.0f, -8.0f);
            camera->setPosition(m_cameraPos);
            camera->lookAt(
                math::Vector3(0.0f, 0.0f, 0.0f), 
                math::Vector3::UNIT_Z
            );
            camera->setAspectRatio((float)getWidth() / (float)getHeight());
        }

        m_context->setCameraHandle(m_camera);

        // Register context with frame renderer
        getCore()->getFrameRenderer()->addContext(m_context);
        
        LOG(LOG_INFO) << "Cube setup complete. Use Arrow Keys to move camera.";
    }

    void update(float deltaTime) override
    {
        // Rotate cube
        m_rotation += deltaTime * 0.5f;
        if (m_rotation > 6.28f)
            m_rotation -= 6.28f;

        poseidon::Renderable* renderable = getCore()->getRenderWorld()->resolveRenderable(m_renderable);
        if (renderable)
        {
            // Create rotation quaternion
            math::Quaternion rot(m_rotation, math::Vector3(0.0f, 1.0f, 0.0f));
            
            // Calculate position with rotation
            math::Vector3 offset(4.0f, 0.0f, 0.0f);
            math::Vector3 rotatedOffset = rot * offset;
            
            // Build transform matrix
            math::Matrix4 transform = math::Matrix4::IDENTITY;
            transform.setRotation(rot);
            transform.setTranslation(rotatedOffset);
            
            renderable->worldTransform = transform;
            renderable->inverseTranspose = transform.inverse().transpose();
        }
    }

    void render() override
    {
        getCore()->iterate();
    }

    bool onInput(const psio::InputEvent& evt) override
    {
        // Camera movement with arrow keys
        bool cameraChanged = false;
        
        if (evt.isKeyPressed(psio::Key::CODE_LEFT))
        {
            m_cameraPos.x -= 0.1f;
            cameraChanged = true;
        }
        if (evt.isKeyPressed(psio::Key::CODE_RIGHT))
        {
            m_cameraPos.x += 0.1f;
            cameraChanged = true;
        }
        if (evt.isKeyPressed(psio::Key::CODE_UP))
        {
            m_cameraPos.z += 0.5f;
            cameraChanged = true;
        }
        if (evt.isKeyPressed(psio::Key::CODE_DOWN))
        {
            m_cameraPos.z -= 0.5f;
            cameraChanged = true;
        }
        if (evt.isKeyPressed(psio::Key::CODE_HOME))
        {
            m_cameraPos.y += 0.5f;
            cameraChanged = true;
        }
        if (evt.isKeyPressed(psio::Key::CODE_END))
        {
            m_cameraPos.y -= 0.5f;
            cameraChanged = true;
        }
        
        if (cameraChanged)
        {
            updateCamera();
            return true;
        }
        
        return false;
    }

    ~CubeSample()
    {
        if (m_renderable != poseidon::InvalidRenderable)
        {
            getCore()->getRenderWorld()->destroyRenderable(m_renderable);
        }
        if (m_camera != poseidon::InvalidCamera)
        {
            getCore()->getRenderWorld()->destroyCamera(m_camera);
        }
        if (m_context)
        {
            delete m_context;
        }
    }

private:
    void updateCamera()
    {
        poseidon::Camera* camera = getCore()->getRenderWorld()->resolveCamera(m_camera);
        if (camera)
        {
            camera->get_position()->setPosition(m_cameraPos);
            camera->get_position()->lookAt(
                math::Vector3(0.0f, 0.0f, 0.0f), 
                math::Vector3::UNIT_Z
            );
        }
    }

    poseidon::Mesh* createCubeMesh(poseidon::Material* material)
    {
        const int num_vertices = 24;
        const int num_faces = 12;

        hippios::vertex_uv vertices[num_vertices] = {
            // Front Face
            {-1.0f, -1.0f, -1.0f, 0.0f, 1.0f}, {-1.0f,  1.0f, -1.0f, 0.0f, 0.0f},
            { 1.0f,  1.0f, -1.0f, 1.0f, 0.0f}, { 1.0f, -1.0f, -1.0f, 1.0f, 1.0f},
            // Back Face
            {-1.0f, -1.0f,  1.0f, 1.0f, 1.0f}, { 1.0f, -1.0f,  1.0f, 0.0f, 1.0f},
            { 1.0f,  1.0f,  1.0f, 0.0f, 0.0f}, {-1.0f,  1.0f,  1.0f, 1.0f, 0.0f},
            // Top Face
            {-1.0f,  1.0f, -1.0f, 0.0f, 1.0f}, {-1.0f,  1.0f,  1.0f, 0.0f, 0.0f},
            { 1.0f,  1.0f,  1.0f, 1.0f, 0.0f}, { 1.0f,  1.0f, -1.0f, 1.0f, 1.0f},
            // Bottom Face
            {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f}, { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f},
            { 1.0f, -1.0f,  1.0f, 0.0f, 0.0f}, {-1.0f, -1.0f,  1.0f, 1.0f, 0.0f},
            // Left Face
            {-1.0f, -1.0f,  1.0f, 0.0f, 1.0f}, {-1.0f,  1.0f,  1.0f, 0.0f, 0.0f},
            {-1.0f,  1.0f, -1.0f, 1.0f, 0.0f}, {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f},
            // Right Face
            { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f}, { 1.0f,  1.0f, -1.0f, 0.0f, 0.0f},
            { 1.0f,  1.0f,  1.0f, 1.0f, 0.0f}, { 1.0f, -1.0f,  1.0f, 1.0f, 1.0f}
        };

        unsigned long indices[] = {
            0,1,2, 0,2,3,    4,5,6, 4,6,7,    8,9,10, 8,10,11,
            12,13,14, 12,14,15,  16,17,18, 16,18,19,  20,21,22, 20,22,23
        };

        poseidon::Mesh* mesh = new poseidon::Mesh();
        mesh->setVertices(sizeof(hippios::vertex_uv), num_vertices, vertices);
        mesh->setMesh(0, sizeof(unsigned long), num_faces, indices, material);
        
        return mesh;
    }
};

int main()
{
    CubeSample app;
    app.init("Poseidon Sample 02: Rotating Cube");
    return app.run();
}
