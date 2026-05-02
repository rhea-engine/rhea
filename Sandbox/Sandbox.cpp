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
#include <Rhea/Geometry/CubeMesh.h>
#include <Rhea/Geometry/PlaneMesh.h>
#include <Rhea/Geometry/SphereMesh.h>
#include <Rhea/Geometry/CylinderMesh.h>
#include <Rhea/Geometry/ConeMesh.h>
#include <Rhea/Geometry/CapsuleMesh.h>
#include <chrono>
#include <Rhea/ObjFile/ObjLoader.h>
#include <Psio/Log.h>


class Sandbox : public rhea::Application
{
private:
    rhea::Scene* m_scene = nullptr;
    rhea::Item* m_cube = nullptr;
    rhea::Item* m_cube2 = nullptr;
    float m_rotation_angle = 0.0f;
    std::chrono::steady_clock::time_point m_last_time;

public:
    void setup() override
    {
        Application::setup();

    }

    void setupScene()
    {

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
        std::string realm_name = "sandbox";
        rhea::Unit* realm_unit = (rhea::Unit*)new rhea::Realm(realm_name);
        m_unit_tree->addChild(realm_unit);

        realm_unit->addChild(m_scene);

        rhea::ResourceList res_list2(this);
        res_list2.push("kies_texture", "//sandbox/kies.png", "//meworla/texture");
        res_list2.push("uv_debug_texture", "//sandbox/uv_debug.png", "//meworla/texture");
        res_list2.push("brick256", "//sandbox/brick256.png", "//meworla/texture");
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



        // Add mesh component        
        rhea::Item* cube2 = simpleparser::Type::create<rhea::Item>("Cube02");
        cube2->setup(this, {}, {});
        rhea::ObjLoader* geometry = simpleparser::Type::create<rhea::ObjLoader>("cube_geometry");
        geometry->setup(this,
            {
                {"path", "//sandbox/cube1.obj"},
                { "material", "//sandbox/cube1.mtl"},
                {"diffuse_tex", "uv_debug_texture"}
            },
            {});

        // If you have a simple mesh, load it:
        rhea::Mesh* loadedMesh = doCast<rhea::Mesh>(getUnitTree()->find("//sandbox/cube_mesh", "", false));
        geometry->load(loadedMesh);
        cube2->addComponentUnique<rhea::VisualComponent>("visual");
        rhea::MeshComponent* loadedMeshComp = cube2->addComponentUnique<rhea::MeshComponent>("mesh");
        loadedMeshComp->setMesh(loadedMesh);
        cube2->setPosition(math::Vector3(3, 1, 0));
        m_scene->addChild(cube2);

		addCube(m_scene);
		addPlane(m_scene);
        addSphere(m_scene);
        addCylinder(m_scene);
		addCone(m_scene);
		addCapsule(m_scene);

        m_scene->ready();
        m_scene->show();
        m_last_time = std::chrono::steady_clock::now();
    }

    void addCube(rhea::Scene* scene)
    {
        // Create cube
            //rhea::Item* cube = m_scene->createUnit<rhea::Item>("Cube");
        rhea::Item* cube = simpleparser::Type::create<rhea::Item>("Cube01");

        // Add visual component (for rendering)
        cube->addComponentUnique<rhea::VisualComponent>("visual");

        // Add mesh component
        rhea::MeshComponent* meshComp = cube->addComponentUnique<rhea::MeshComponent>("mesh");

        // Or create a simple procedural mesh
        rhea::CubeMesh* cubeMesh = simpleparser::Type::create<rhea::CubeMesh>("cube_mesh");
        cube->addChild(cubeMesh);
        cubeMesh->setSize(0.75f);
        cubeMesh->createMesh();
        meshComp->setMesh(cubeMesh);
        cube->setLookAt(math::Vector3(5.0f, 5.0f, 0), math::Vector3::UNIT_Z);
        scene->addChild(cube);
        m_cube = cube; // Store reference to m_cube
        m_cube->setPosition(math::Vector3(0.0f, 1.0f, 0.0f));

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
        //rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/kies.png", "", false));
        //rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/uv_debug.png", "", false));
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/brick256.png", "", false));
        material->setTexture(0, image_res); // Add texture

        scene->addChild(material);
        cubeMesh->setMaterial(material);    
    }

    void addPlane(rhea::Scene* scene)
    {
        // Create plane
        rhea::Item* plane = simpleparser::Type::create<rhea::Item>("Plane");
        plane->addComponentUnique<rhea::VisualComponent>("visual");
        rhea::MeshComponent* meshComp = plane->addComponentUnique<rhea::MeshComponent>("mesh");
        // Create a simple procedural plane mesh
        rhea::PlaneMesh* planeMesh = simpleparser::Type::create<rhea::PlaneMesh>("plane_mesh");
        planeMesh->setSize(4.0f, 4.0f);
        planeMesh->setSegments(4, 4);
		planeMesh->setUVScale(2.0f, 2.0f); // Repeat texture
        planeMesh->createMesh();
        meshComp->setMesh(planeMesh);
        scene->addChild(plane);

		// add material
        rhea::Material* material = simpleparser::Type::create<rhea::Material>("plane_material");
        material->setup(this);
        rhea::Shader* shader_res = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
        material->setValue("shader", "standard");
        if (shader_res) {
            material->setShaderRes(shader_res);
        }
        // Material with gray color
        material->setColor(0, math::Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Vector4(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Vector4(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
        material->setFloat(0, 16.0f); // Shininess for highlights
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/brick256.png", "", false));
        material->setTexture(0, image_res); // Add texture
        scene->addChild(material);
		planeMesh->setMaterial(material);
	}

    void addSphere(rhea::Scene* scene)
    {
        // Create sphere
        rhea::Item* sphere = simpleparser::Type::create<rhea::Item>("Sphere");
        sphere->addComponentUnique<rhea::VisualComponent>("visual");
        rhea::MeshComponent* meshComp = sphere->addComponentUnique<rhea::MeshComponent>("mesh");
        // Create a simple procedural sphere mesh
        rhea::SphereMesh* sphereMesh = simpleparser::Type::create<rhea::SphereMesh>("sphere_mesh");
        sphereMesh->setRadius(1.0f);
        sphereMesh->setSegments(16);
        sphereMesh->createMesh();
        meshComp->setMesh(sphereMesh);
        scene->addChild(sphere);
		sphere->setPosition(math::Vector3(-2.0f, 1.0f, 2.0f));

        // add material
        rhea::Material* material = simpleparser::Type::create<rhea::Material>("sphere_material");
        material->setup(this);
        rhea::Shader* shader_res = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
        material->setValue("shader", "standard");
        if (shader_res) {
            material->setShaderRes(shader_res);
        }
        // Material with gray color
        material->setColor(0, math::Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Vector4(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Vector4(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
        material->setFloat(0, 16.0f); // Shininess for highlights
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/brick256.png", "", false));
        material->setTexture(0, image_res); // Add texture
        scene->addChild(material);
        sphereMesh->setMaterial(material);
    }

    void addCylinder(rhea::Scene* scene)
    {
        // Create cylinder
        rhea::Item* cylinder = simpleparser::Type::create<rhea::Item>("Cylinder");
        cylinder->addComponentUnique<rhea::VisualComponent>("visual");
        rhea::MeshComponent* meshComp = cylinder->addComponentUnique<rhea::MeshComponent>("mesh");
        // Create a simple procedural cylinder mesh
        rhea::CylinderMesh* cylinderMesh = simpleparser::Type::create<rhea::CylinderMesh>("cylinder_mesh");
        cylinderMesh->setRadius(0.5f);
        cylinderMesh->setHeight(2.0f);
        cylinderMesh->setSegments(16);
        cylinderMesh->createMesh();
        meshComp->setMesh(cylinderMesh);
        scene->addChild(cylinder);
        cylinder->setPosition(math::Vector3(2.0f, 1.0f, 2.0f));

        // add material
        rhea::Material* material = simpleparser::Type::create<rhea::Material>("cylinder_material");
        material->setup(this);
        rhea::Shader* shader_res = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
        material->setValue("shader", "standard");
        if (shader_res) {
            material->setShaderRes(shader_res);
        }
        // Material with gray color
        material->setColor(0, math::Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Vector4(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Vector4(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
        material->setFloat(0, 16.0f); // Shininess for highlights
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/brick256.png", "", false));
        material->setTexture(0, image_res); // Add texture
        scene->addChild(material);
        cylinderMesh->setMaterial(material);
    }

    void addCone(rhea::Scene* scene)
    {
        // Create cone
        rhea::Item* cone = simpleparser::Type::create<rhea::Item>("Cone");
        cone->addComponentUnique<rhea::VisualComponent>("visual");
        rhea::MeshComponent* meshComp = cone->addComponentUnique<rhea::MeshComponent>("mesh");
        // Create a simple procedural cone mesh
        rhea::ConeMesh* coneMesh = simpleparser::Type::create<rhea::ConeMesh>("cone_mesh");
        coneMesh->setRadius(0.5f);
        coneMesh->setHeight(2.0f);
        coneMesh->setSegments(16);
        coneMesh->setCap(true);
        coneMesh->createMesh();
        meshComp->setMesh(coneMesh);
        scene->addChild(cone);
        cone->setPosition(math::Vector3(-1.25f, 1.0f, 2.0f));
        // add material (similar to previous examples)

        // add material
        rhea::Material* material = simpleparser::Type::create<rhea::Material>("cone_material");
        material->setup(this);
        rhea::Shader* shader_res = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
        material->setValue("shader", "standard");
        if (shader_res) {
            material->setShaderRes(shader_res);
        }
        // Material with gray color
        material->setColor(0, math::Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Vector4(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Vector4(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
        material->setFloat(0, 16.0f); // Shininess for highlights
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/brick256.png", "", false));
        material->setTexture(0, image_res); // Add texture
        scene->addChild(material);
        coneMesh->setMaterial(material);

	}

    void addCapsule(rhea::Scene* scene)
    {
        // Create capsule
        rhea::Item* capsule = simpleparser::Type::create<rhea::Item>("Capsule");
        capsule->addComponentUnique<rhea::VisualComponent>("visual");
        rhea::MeshComponent* meshComp = capsule->addComponentUnique<rhea::MeshComponent>("mesh");
        // Create a simple procedural capsule mesh
        rhea::CapsuleMesh* capsuleMesh = simpleparser::Type::create<rhea::CapsuleMesh>("capsule_mesh");
        capsuleMesh->setRadius(0.5f);
        capsuleMesh->setHeight(2.0f);
        capsuleMesh->setSegments(16);
        capsuleMesh->createMesh();
        meshComp->setMesh(capsuleMesh);
        scene->addChild(capsule);
        capsule->setPosition(math::Vector3(1.0f, 1.0f, 2.0f));

        // add material
        rhea::Material* material = simpleparser::Type::create<rhea::Material>("capsule_material");
        material->setup(this);
        rhea::Shader* shader_res = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
        material->setValue("shader", "standard");
        if (shader_res) {
            material->setShaderRes(shader_res);
        }
        // Material with gray color
        material->setColor(0, math::Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Vector4(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Vector4(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
        material->setFloat(0, 16.0f); // Shininess for highlights
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/brick256.png", "", false));
        material->setTexture(0, image_res); // Add texture
        scene->addChild(material);
        capsuleMesh->setMaterial(material);
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
        m_last_time = std::chrono::steady_clock::now();

        LOG(LOG_INFO) << "Scene ready";
        rhea::Event ev_r("ready", scene);
        scene->sendEvent(std::string("ready"), &ev_r);
    }

    void showScene(rhea::Scene* scene)
    {
		m_cube2 = doCast<rhea::Item>(scene->find("cube1"));
		addCube(scene);
        scene->show();
    }

    bool onFrameBegin() override
    {
        // Delta-Time berechnen
        auto current_time = std::chrono::steady_clock::now();
        float delta_time = std::chrono::duration<float>(current_time - m_last_time).count();
        m_last_time = current_time;

        // Rotation mit konstanter Geschwindigkeit (Grad pro Sekunde)
        float rotation_speed = 45.0f; // 45 Grad pro Sekunde
        m_rotation_angle += rotation_speed * delta_time * (3.14159f / 180.0f);

        float radius = 5.0f;
        math::Vector3 target(
            radius * std::sin(m_rotation_angle),            
            0.0f,
            radius * std::cos(m_rotation_angle)
        );
        target = math::Vector3(
            radius * std::sin(m_rotation_angle),
            radius * std::cos(m_rotation_angle) * 0.5f, // Leichte Z-Komponente
            radius * std::cos(m_rotation_angle)
        );

        if (m_cube)
            m_cube->setLookAt(target, math::Vector3::UNIT_Y);
        if (m_cube2)
            m_cube2->setLookAt(target, math::Vector3::UNIT_Y);        

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
    Sandbox app;
    app.setAppname("Sandbox");
    app.init(argc, argv);
    app.setup();

    std::string home_scene = "//sandbox/world.scene";
    if (1) {
        app.setupScene();
    }
    else {
        app.loadResource(home_scene, "meworla/scene", [&](rhea::Unit* scene, ReqError error) {
            rhea::Scene* world_scene = doCast<rhea::Scene>(scene);
            if (world_scene) {
                app.showScene(world_scene);
            }
            else {
                LOG(LOG_ERROR) << "Failed to load world scene";
            }
            return 0;
            });
    }
    return app.loop();
}