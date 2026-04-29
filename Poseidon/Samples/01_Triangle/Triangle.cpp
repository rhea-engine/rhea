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
#include <Poseidon/Viewport.h>
#include <Poseidon/FrameRenderer.h>
#include <Hippios/VertexBuffer.h>
#include <Psio/Log.h>

using namespace poseidon::samples;

/**
 * @brief Minimal Triangle Sample
 *
 * Zeigt:
 * - Einfachste Poseidon-Geometrie (3 Vertices)
 * - Shader laden
 * - Material erstellen
 * - Mesh mit Vertex-Farben
 */
class TriangleSample : public PoseidonApp
{
private:
    poseidon::Mesh* m_mesh = nullptr;
    poseidon::Material* m_material = nullptr;

public:
    void setup() override
    {
        LOG(LOG_INFO) << "Setting up Triangle sample...";

        // Erstelle Shader
        poseidon::Shader* shader = getCore()->createShader("basic");
        if (!shader->loadFromFile("Data/Rhea/shaders/shader.hlsl", "VShader", "PShader"))
        {
            LOG(LOG_ERROR) << "Failed to load shader!";
            m_running = false;
            return;
        }

        // Erstelle Material
        m_material = getCore()->createMaterial();
        m_material->setShader(shader);

        // Erstelle Mesh
        m_mesh = new poseidon::Mesh();
        m_mesh->setViewport(getCore()->getFrameRenderer()->getViewport());

        // Triangle vertices (Position + Color)
        hippios::vertex vertices[] = {
            { 0.0f,  0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f} },   // Top (Red)
            { 0.45f, -0.5f, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f} },  // Right (Green)
            {-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f} }   // Left (Blue)
        };

        m_mesh->setVertices(sizeof(hippios::vertex), 3, vertices);
		unsigned long indices[] = { 0, 1, 2 };
		m_mesh->setMesh(0, sizeof(unsigned long), 3, indices, m_material);

        // Add to scene
        getCore()->getFrameRenderer()->getViewport()->getRenderContext("default")->addRenderChild(m_mesh);

        LOG(LOG_INFO) << "Triangle setup complete";
    }

    void render() override
    {
        getCore()->iterate();
    }
};

int main()
{
    TriangleSample app;
    app.init("Poseidon Sample 01: Colored Triangle");
    return app.run();
}