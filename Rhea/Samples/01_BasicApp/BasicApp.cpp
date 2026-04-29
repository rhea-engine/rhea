/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Application.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Core/Scene.h"
#include "Rhea/Core/Item.h"
#include "Rhea/Core/Camera.h"
#include "Rhea/Core/Mesh.h"
#include "Rhea/Components/MeshComponent.h"
#include "Rhea/Components/VisualComponent.h"
#include <Rhea/EventInput.h>
#include <Rhea/Input/InputEvent.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>
#include <Rhea/Realm.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include "Rhea/Core/DirectionalLight.h"
#include <Rhea/Core/Lighting.h>
#include <Rhea/Resources/ImageResource.h>

class BasicApp : public rhea::Application
{
private:
    rhea::Scene* m_scene = nullptr;

public:
    void setup() override
    {
        Application::setup();

        // add graphics service
        // load core resources
        rhea::ResourceList res_list(this);

        // load shaders
        res_list.push("standard_shader", "//rhea/shaders/standard.shader", "//meworla/shader");       

        // note: for locally installed resources this is actually synchronous
        static_cast<rhea::ResourceService*>(getService("resources"))->loadAsync(res_list, nullptr);

        // Create a minimal scene
        m_scene = simpleparser::Type::create<rhea::Scene>("scene");
        m_scene->setup(this);

        // add the realm
        std::string realm_name = "basic_app";
        rhea::Unit* realm_unit = (rhea::Unit*)new rhea::Realm(realm_name);
        m_unit_tree->addChild(realm_unit);

        realm_unit->addChild(m_scene);

        rhea::ResourceList res_list2(this);
        res_list2.push("kies_texture", "//basic_app/kies.png", "//meworla/texture");
        static_cast<rhea::ResourceService*>(getService("resources"))->loadAsync(res_list2, nullptr);

        // Create camera
        //rhea::Camera* camera = m_scene->createUnit<rhea::Camera>("MainCamera");
        rhea::Camera* camera = simpleparser::Type::create<rhea::Camera>("MainCamera");
        camera->setPosition(math::Vector3(0, 2, -5));
        camera->setLookAt(math::Vector3(0, 0, 0), math::Vector3::UNIT_Z);
        camera->setup(this, {}, {});
        //m_scene->setMainCamera(camera);
        m_scene->addChild(camera);

        
        rhea::DirectionalLight* light = simpleparser::Type::create<rhea::DirectionalLight>("MainLight");
        light->setDirection(math::Vector3(-0.5f, -1.0f, -0.5f));
        light->setColor(math::Vector3(1.0f, 1.0f, 1.0f));
        light->setDiffuse(math::Quaternion(1.0f, 1.0f, 1.0f, 1.0f));
        m_scene->addChild(light);
        
        rhea::Lighting* lighting = simpleparser::Type::create<rhea::Lighting>("MainLighting");
        lighting->setAmbient(math::Quaternion(0.8f, 0.8f, 0.8f, 1.0f));
        lighting->setup(this, {}, {});
        m_scene->addChild(lighting);

        // Create cube
        //rhea::Item* cube = m_scene->createUnit<rhea::Item>("Cube");
        rhea::Item* cube = simpleparser::Type::create<rhea::Item>("Cube01");
        
        // Add visual component (for rendering)
        cube->addComponentUnique<rhea::VisualComponent>("visual");
        
        // Add mesh component
        rhea::MeshComponent* meshComp = cube->addComponentUnique<rhea::MeshComponent>("mesh");
        
        // If you have a simple mesh, load it:
        // meshComp->setMesh("//assets/cube.obj");
        
        // Or create a simple procedural mesh
        rhea::Mesh* cubeMesh = createSimpleCube();
        cube->addChild(cubeMesh);
        meshComp->setMesh(cubeMesh);
        m_scene->addChild(cube);

        rhea::Material* material = simpleparser::Type::create<rhea::Material>("cube_material");
        material->setup(this);
        rhea::Shader* shader_res = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
        material->setValue("shader", "standard");
        if (shader_res) {
            material->setShaderRes(shader_res);
        }
        // Material with red color
        // Set colors
        material->setColor(0, math::Vector4(0.8f, 0.1f, 0.1f, 1.0f)); // Diffuse: Red
        material->setColor(1, math::Vector4(0.2f, 0.0f, 0.0f, 1.0f)); // Ambient: Dark red
        material->setColor(2, math::Vector4(1.0f, 0.5f, 0.5f, 1.0f)); // Specular: Light red/pink
        material->setFloat(0, 32.0f); // Shininess for highlights
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//basic_app/kies.png", "", false));
        material->setTexture(0, image_res); // Add texture

        m_scene->addChild(material);
        cubeMesh->setMaterial(material);

        m_scene->ready();
        m_scene->show();
    }
    
    bool onInput(rhea::EventInput* e, rhea::Unit* unit) override
    {
        rhea::InputEvent evt = e->getEvent();

        if (evt.isKeyPressed(rhea::Key::Escape)) {
            // user confirmed quit
            m_exiting = true;
            return true;
        }
        
        return Application::onInput(e, unit);
    }

    void onScene(rhea::Scene* scene) override
    {        
		// do nothing, we set the scene in setup
	}

    bool onFrameBegin() override
	{
		return false;
	}

    bool onFrameEnd() override
    {
        return m_exiting;
    }

private:
	bool m_exiting = false;

    rhea::Mesh* createSimpleCube()
    {
        rhea::Mesh* mesh = simpleparser::Type::create<rhea::Mesh>("cube_mesh");
        mesh->setup(this, {}, {});

        // Einfache Würfel-Vertices (Position + Texcoord)
        std::vector<rhea::Mesh::vertex> vertices = {
            // Front Face
            { -1.0f, -1.0f, -1.0f, 0.0f, 1.0f },
            { -1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
            {  1.0f,  1.0f, -1.0f, 1.0f, 0.0f },
            {  1.0f, -1.0f, -1.0f, 1.0f, 1.0f },

            // Back Face
            { -1.0f, -1.0f,  1.0f, 1.0f, 1.0f },
            {  1.0f, -1.0f,  1.0f, 0.0f, 1.0f },
            {  1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
            { -1.0f,  1.0f,  1.0f, 1.0f, 0.0f },

            // Top Face
            { -1.0f,  1.0f, -1.0f, 0.0f, 1.0f },
            { -1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
            {  1.0f,  1.0f,  1.0f, 1.0f, 0.0f },
            {  1.0f,  1.0f, -1.0f, 1.0f, 1.0f },

            // Bottom Face
            { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
            {  1.0f, -1.0f, -1.0f, 0.0f, 1.0f },
            {  1.0f, -1.0f,  1.0f, 0.0f, 0.0f },
            { -1.0f, -1.0f,  1.0f, 1.0f, 0.0f },

            // Left Face
            { -1.0f, -1.0f,  1.0f, 0.0f, 1.0f },
            { -1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
            { -1.0f,  1.0f, -1.0f, 1.0f, 0.0f },
            { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },

            // Right Face
            {  1.0f, -1.0f, -1.0f, 0.0f, 1.0f },
            {  1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
            {  1.0f,  1.0f,  1.0f, 1.0f, 0.0f },
            {  1.0f, -1.0f,  1.0f, 1.0f, 1.0f }
        };

        // Indices für Dreiecke
        std::vector<uint32_t> indices = {
            // Front
            0, 1, 2,    0, 2, 3,
            // Back
            4, 5, 6,    4, 6, 7,
            // Top
            8, 9, 10,   8, 10, 11,
            // Bottom
            12, 13, 14, 12, 14, 15,
            // Left
            16, 17, 18, 16, 18, 19,
            // Right
            20, 21, 22, 20, 22, 23
        };

        mesh->setBuffers(vertices, indices);
        
        return mesh;
    }
};

int main(int argc, char** argv)
{
    BasicApp app;
	app.setAppname("BasicApp");
	app.init(argc, argv);
    app.setup();
    return app.loop();
}