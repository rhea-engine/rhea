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
#include <Rhea/Error.h>
#include <Psio/Log.h>
#include <cmath>
#include <Math/Math.h>
#include <Rhea/Realm.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include "Rhea/Core/DirectionalLight.h"
#include <Rhea/Core/Lighting.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Geometry/CubeMesh.h>

class BasicApp : public rhea::Application
{
private:
    rhea::Scene* m_scene = nullptr;
    rhea::Item* m_cube = nullptr;
    float m_cube_angle = 0.0f;
    rhea::ResourceList m_resources{this};

public:
    void setup() override
    {
        Application::setup();

        // Application::setup() has already instantiated the standard shader.
        m_resources.push("kies_texture", "//basic_app/kies.png", "//meworla/texture");
        m_resources.load([this](rhea::Error* error) {
            if (error) {
                LOG(LOG_ERROR) << "Failed to load BasicApp assets: " << error->getText();
                m_exiting = true;
                return;
            }
            auto* shader = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
            auto* texture = doCast<rhea::ImageResource>(m_resources.get("kies_texture"));
            if (!shader || !texture) {
                LOG(LOG_ERROR) << "BasicApp requires the standard shader and kies.png. Check the Data directory.";
                m_exiting = true;
                return;
            }
            createScene(shader, texture);
        });
    }

private:
    void createScene(rhea::Shader* shader, rhea::ImageResource* texture)
    {
        // Create a minimal scene
        m_scene = simpleparser::Type::create<rhea::Scene>("scene");
        m_scene->setup(this, {}, {});

        // Resource loading has already created the application asset realm.
        rhea::Unit* realm_unit = doCast<rhea::Unit>(m_unit_tree->find("//basic_app", "", false));
        realm_unit->addChild(m_scene);

        // Create camera
        //rhea::Camera* camera = m_scene->createUnit<rhea::Camera>("MainCamera");
        rhea::Camera* camera = simpleparser::Type::create<rhea::Camera>("MainCamera");
        camera->setup(this, {}, {});
        camera->setLocalPosition(math::Vector3(0, 2, -7.5f));
        camera->setLookAt(math::Vector3(0, 0, 0), math::Vector3::UNIT_Z);
        //m_scene->setMainCamera(camera);
        m_scene->addChild(camera);

        
        rhea::DirectionalLight* light = simpleparser::Type::create<rhea::DirectionalLight>("MainLight");
        light->setup(this, {}, {});
        light->setDirection(math::Vector3(-0.5f, -1.0f, 0.5f).normalizedCopy());
        light->setColor(math::Color(1.0f, 1.0f, 1.0f, 1.0f));
        light->setDiffuse(math::Color(1.0f, 1.0f, 1.0f, 1.0f));
        m_scene->addChild(light);
                
        rhea::Lighting* lighting = simpleparser::Type::create<rhea::Lighting>("MainLighting");
        lighting->setAmbient(math::Color(0.25f, 0.25f, 0.25f, 1.0f));
        lighting->setup(this, {}, {});
        m_scene->addChild(lighting);
        
        // Create cube
        //rhea::Item* cube = m_scene->createUnit<rhea::Item>("Cube");
        rhea::Item* cube = simpleparser::Type::create<rhea::Item>("Cube01");
        m_cube = cube;
        cube->setup(this, {}, {});
		cube->setLocalPosition(math::Vector3(0, 0, 0));


        // Add visual component (for rendering)
        cube->addComponentUnique<rhea::VisualComponent>("visual");

        // Add mesh component
        rhea::MeshComponent* mesh_component = cube->addComponentUnique<rhea::MeshComponent>("mesh");

        // Create a simple procedural mesh
        rhea::CubeMesh* cube_mesh = simpleparser::Type::create<rhea::CubeMesh>("cube_mesh");
        cube_mesh->setup(this, {}, {});
        cube->addChild(cube_mesh);
        cube_mesh->createMesh();
        mesh_component->setMesh(cube_mesh);
        m_scene->addChild(cube);

        rhea::Material* material = simpleparser::Type::create<rhea::Material>("cube_material");
        material->setup(this, {}, {});
        material->setShader(shader);
        material->setColor(shader->getInputSlotId("AMBIENT"), math::Color(1.0f, 1.0f, 1.0f, 1.0f));
        material->setColor(shader->getInputSlotId("DIFFUSE"), math::Color(1.0f, 1.0f, 1.0f, 1.0f));
        material->setColor(shader->getInputSlotId("SPECULAR"), math::Color(0.1f, 0.1f, 0.1f, 1.0f));
        material->setFloat(shader->getInputSlotId("SHININESS"), 32.0f);
        material->setTexture(shader->getTextureSlotId("BASECOLOR"), texture);

        m_scene->addChild(material);
        cube_mesh->setMaterial(material);

        m_scene->ready();
        m_scene->show();
        LOG(LOG_INFO) << "BasicApp scene ready: kies.png (" << texture->width << " x " << texture->height << ").";
    }

public:
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
        if (m_cube) {
            const float delta_seconds = static_cast<float>(elapsed_ticks) / 1000000.0f;
            m_cube_angle = std::fmod(m_cube_angle + 30.0f * delta_seconds, 360.0f);
            rhea::Position position = m_cube->getTransform()->getLocalPosition();
            position.setRotation(math::Quaternion(m_cube_angle * math::degToRad, math::Vector3::UNIT_Y));
            m_cube->setLocalPosition(position);
        }
        return false;
    }

    bool onFrameEnd() override
    {
        return m_exiting;
    }

private:
    bool m_exiting = false;
};
    

int main(int argc, char** argv)
{
    BasicApp app;
	app.setAppname("BasicApp");
	app.init(argc, argv);
    app.setup();
    return app.loop();
}
