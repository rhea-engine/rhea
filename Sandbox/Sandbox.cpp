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
#include <Rhea/Geometry/TorusMesh.h>
#include <chrono>
#include <Rhea/ObjFile/ObjLoader.h>
#include <Psio/Log.h>
#include <Rhea/DebugDraw/LineBatchMesh.h>
#include <Math/Matrix3x4.h>
#include <Rhea/Text/BitmapFont.h>

// GUI includes
#include <Rhea/GUI/GUIController.h>
#include <Rhea/GUI/ScrollView.h>
#include <Rhea/GUI/Column.h>
#include <Rhea/GUI/Label.h>
#include <Rhea/GUI/Button.h>
#include <Rhea/GUI/Box.h>
#include <Rhea/GUI/GuiService.h>
#include <Rhea/GUI/Styles/StyleSheet.h>
#include <Rhea/Graphics/SizeEvent.h>
#include <SimpleParser/Variant.h>


class Sandbox : public rhea::Application
{
private:
    rhea::Scene* m_scene = nullptr;
    rhea::Item* m_cube = nullptr;
    rhea::Item* m_cube2 = nullptr;
    rhea::Item* m_gizmo = nullptr;
    rhea::GUIController* m_gui_controller = nullptr;
    rhea::ScrollView* m_scrollview = nullptr;
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
        res_list.push("text_shader", "//rhea/shaders/text.shader", "//meworla/shader");

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

        // add fonts
		rhea::BitmapFont* font = simpleparser::Type::create<rhea::BitmapFont>("Arial20");
        font->setup(this, {}, {
            { "texture", "//sandbox/Arial20.bmp" }, 
            { "font-info", "//sandbox/Arial20.csv" },
            { "font-size", "22"}
            });


        rhea::ResourceList res_list2(this);
        res_list2.push("kies_texture", "//sandbox/kies.png", "//meworla/texture");
        res_list2.push("uv_debug_texture", "//sandbox/uv_debug.png", "//meworla/texture");
        res_list2.push("brick256", "//sandbox/brick256.png", "//meworla/texture");
        res_list2.push("grass256", "//sandbox/grass256.png", "//meworla/texture");
		auto refs = font->getReferences();
        for (auto ref : refs) {
            res_list2.push(ref.name, ref.value.toString(), ref.type);
        }
        static_cast<rhea::ResourceService*>(getService("resources"))->loadAsync(res_list2, nullptr);

        font->ready();
        m_scene->addChild(font);

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
        light->setColor(math::Color(1.0f, 1.0f, 1.0f));
        light->setDiffuse(math::Color(1.0f, 1.0f, 1.0f, 1.0f));
        m_scene->addChild(light);

        rhea::Lighting* lighting = simpleparser::Type::create<rhea::Lighting>("MainLighting");
        lighting->setAmbient(math::Color(0.8f, 0.8f, 0.8f, 1.0f));
        lighting->setup(this, {}, {});
        m_scene->addChild(lighting);


/*
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
        */

		addCube(m_scene);
		addPlane(m_scene);
		addSphere(m_scene);
		addCylinder(m_scene);
		addCone(m_scene);
		addCapsule(m_scene);
		addTorus(m_scene);

		addGizmo(m_scene);

		m_scene->ready();

		// Setup GUI after scene is ready
		setupGUI();

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
        cube->setLookAt(math::Vector3(0.0f, 0.0f, 0), math::Vector3::UNIT_Z);
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
        material->setColor(0, math::Color(0.8f, 0.1f, 0.1f, 1.0f)); // Diffuse: Red
        material->setColor(1, math::Color(0.2f, 0.0f, 0.0f, 1.0f)); // Ambient: Dark red
        material->setColor(2, math::Color(1.0f, 0.5f, 0.5f, 1.0f)); // Specular: Light red/pink
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
        material->setColor(0, math::Color(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Color(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Color(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
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
        material->setColor(0, math::Color(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Color(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Color(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
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
        cylinderMesh->setHeight(1.0f);
        cylinderMesh->setSegments(16);
        cylinderMesh->createMesh();
        meshComp->setMesh(cylinderMesh);
        scene->addChild(cylinder);
        cylinder->setPosition(math::Vector3(1.0f, 1.0f, -2.0f));

        // add material
        rhea::Material* material = simpleparser::Type::create<rhea::Material>("cylinder_material");
        material->setup(this);
        rhea::Shader* shader_res = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
        material->setValue("shader", "standard");
        if (shader_res) {
            material->setShaderRes(shader_res);
        }
        // Material with gray color
        material->setColor(0, math::Color(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Color(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Color(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
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
        material->setColor(0, math::Color(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Color(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Color(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
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
        material->setColor(0, math::Color(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Color(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Color(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
        material->setFloat(0, 16.0f); // Shininess for highlights
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/brick256.png", "", false));
        material->setTexture(0, image_res); // Add texture
        scene->addChild(material);
        capsuleMesh->setMaterial(material);
	}

    void addTorus(rhea::Scene* scene)
    {
        // Create torus
        rhea::Item* torus = simpleparser::Type::create<rhea::Item>("Torus");
        torus->addComponentUnique<rhea::VisualComponent>("visual");
        rhea::MeshComponent* meshComp = torus->addComponentUnique<rhea::MeshComponent>("mesh");
        // Create a simple procedural torus mesh
        rhea::TorusMesh* torusMesh = simpleparser::Type::create<rhea::TorusMesh>("torus_mesh");
        torusMesh->setMajorRadius(0.5f);
        torusMesh->setMinorRadius(0.2f);
        torusMesh->setResolution(16, 8);
        torusMesh->createMesh();
        meshComp->setMesh(torusMesh);
        scene->addChild(torus);
        torus->setPosition(math::Vector3(1.0f, 1.0f, -1.5f));
        
        // add material
        rhea::Material* material = simpleparser::Type::create<rhea::Material>("torus_material");
        material->setup(this);
        rhea::Shader* shader_res = doCast<rhea::Shader>(getUnitTree()->find("//rhea/shaders/standard", "", false));
        material->setValue("shader", "standard");
        if (shader_res) {
            material->setShaderRes(shader_res);
        }
        // Material with gray color
        material->setColor(0, math::Color(0.5f, 0.5f, 0.5f, 1.0f)); // Diffuse: Gray
        material->setColor(1, math::Color(0.2f, 0.2f, 0.2f, 1.0f)); // Ambient: Dark gray
        material->setColor(2, math::Color(0.8f, 0.8f, 0.8f, 1.0f)); // Specular: Light gray
        material->setFloat(0, 16.0f); // Shininess for highlights
        rhea::ImageResource* image_res = doCast<rhea::ImageResource>(m_unit_tree->find("//sandbox/brick256.png", "", false));
        material->setTexture(0, image_res); // Add texture
        scene->addChild(material);
        torusMesh->setMaterial(material);
	}

    void addGizmo(rhea::Scene* scene)
    {
		// Create a simple gizmo (e.g., three lines) to visualize the target point of the cube's look-at
        m_gizmo = simpleparser::Type::create<rhea::Item>("Gizmo");
        m_gizmo->addComponentUnique<rhea::VisualComponent>("visual");
        rhea::MeshComponent* meshComp = m_gizmo->addComponentUnique<rhea::MeshComponent>("mesh");
        rhea::LineBatchMesh *lineMesh = simpleparser::Type::create<rhea::LineBatchMesh>("gizmo_mesh");
        lineMesh->setup(this, {}, {});
        lineMesh->ready();
        
        rhea::Position pos = *m_cube->getTransform()->get_derived_position();
        math::Matrix3x4 m = pos.getWorldTransform();
        lineMesh->addAxis(m, 1.0f);
        lineMesh->update();
        meshComp->setMesh(lineMesh);
        scene->addChild(m_gizmo);
        
		rhea::Item* cubeLineItem = simpleparser::Type::create<rhea::Item>("CubeLineItem");
		cubeLineItem->addComponentUnique<rhea::VisualComponent>("visual");
		rhea::MeshComponent* cubeLineMeshComp = cubeLineItem->addComponentUnique<rhea::MeshComponent>("mesh");
		rhea::LineBatchMesh* cubeLineMesh = simpleparser::Type::create<rhea::LineBatchMesh>("cube_line_mesh");
		cubeLineMesh->setup(this, {}, {});
		cubeLineMesh->ready();
		cubeLineMesh->addCube(math::Vector3(0.1f, 0.1f, 0.1f), math::Vector3(0.9f, 0.9f, 0.9f), math::Vector4(1, 1, 1, 1));
		cubeLineMesh->update();
		cubeLineMeshComp->setMesh(cubeLineMesh);
		cubeLineItem->setPosition(math::Vector3(-2.0f, 0.2f, -1.0f));
		scene->addChild(cubeLineItem);

        // add some lines 
		rhea::Item* linesItem = simpleparser::Type::create<rhea::Item>("LineItem");  
		linesItem->addComponentUnique<rhea::VisualComponent>("visual");
		rhea::MeshComponent* linesMeshComp = linesItem->addComponentUnique<rhea::MeshComponent>("mesh");
		rhea::LineBatchMesh* linesMesh = simpleparser::Type::create<rhea::LineBatchMesh>("lines_mesh"); 
        linesMesh->setup(this, {}, {});
        linesMesh->ready();
        linesMesh->addLine({ 0,0,0 }, { 1,0,0 }, { 1,0,0,1 });
        linesMesh->addLine({ 0,0,0 }, { 0,1,0 }, { 0,1,0,1 });
        linesMesh->addLine({ 0,0,0 }, { 1,0,1 }, { 0,0,1,1 });
        linesMesh->addCube(math::Vector3(1.1f, 0.5f, -1.0f), math::Vector3(2.0f, 1.4f, -0.1f), math::Vector4(1, 1, 1, 1));
        linesMesh->addAxis(math::Vector3(-1.0f, 1.0f, 0.0f), 0.5);
        rhea::Position pos_grid;
        linesMesh->addGrid(pos_grid.getWorldTransform(), 5, 50, math::Vector4(1, 0, 0, 1));
        linesMesh->update();
		linesMeshComp->setMesh(linesMesh);
		scene->addChild(linesItem);
	}

	void setupGUI()
	{
		loadResource("//sandbox/sandbox.css", "//meworla/stylesheet", [&](rhea::Unit* unit, ReqError error) {
			// Handle the loaded resource
			if (error == ReqError::OK) {
				LOG(LOG_INFO) << "Stylesheet loaded successfully: " << unit->getPathName();
			}
			else {
				LOG(LOG_ERROR) << "Failed to load stylesheet: " << unit->getPathName() << ", error: " << static_cast<int>(error);
			}
		});

		// Create minimal stylesheet for GUI
	/*	rhea::StyleSheet* stylesheet = simpleparser::Type::create<rhea::StyleSheet>("gui_stylesheet");
		stylesheet->setup(this, {}, {});  
		rhea::Unit* realm_unit = (rhea::Unit*)m_unit_tree->find("//sandbox", "", false);
		if (realm_unit) {
			realm_unit->addChild(stylesheet);
		}
		else
			return;
            */

		// Create GUIController
		m_gui_controller = simpleparser::Type::create<rhea::GUIController>("gui_controller");
		m_gui_controller->setup(this, 
			{{"basepath", "//sandbox"}}, 
			{{"stylesheet", "//sandbox/sandbox.css"},
			//{"texture", "//sandbox/kies.png"},       
            {"font", "//sandbox/scene/Arial20"}});
		m_scene->addChild(m_gui_controller);

		// Create ScrollView container (Box for styling)
		rhea::Widget* scrollview_box = simpleparser::Type::create<rhea::Widget>("scrollviewbox");
		scrollview_box->setup(this, {}, {
			{"x", Variant(50.0f)},
			{"y", Variant(50.0f)},
			{"w", Variant(300.0f)},
			{"h", Variant(400.0f)},                    
		});
		m_gui_controller->addChild(scrollview_box);

		// Create ScrollView
		m_scrollview = simpleparser::Type::create<rhea::ScrollView>("testscrollview");
		m_scrollview->setup(this, {}, {
			{"overflow", Variant("scroll")},
			{"scroll_distance", Variant(30.0f)}
		});
		scrollview_box->addChild(m_scrollview);

		// Create Column as content container
		rhea::ColumnWidget* column = simpleparser::Type::create<rhea::ColumnWidget>("scroll_content");
		column->setup(this, {}, {});
		m_scrollview->addChild(column);

		// Add test items to ScrollView
		for (int i = 0; i < 20; ++i) {
			// Create Box for each item
			rhea::Widget* item_box = simpleparser::Type::create<rhea::Widget>("item_box_" + std::to_string(i));
			if (i % 2 == 0) {				
                item_box->setup(this, {}, {
                {"w", Variant(280.0f)},
                {"h", Variant(60.0f)},
                 {"texture", "//sandbox/brick256.png"},
                    });
                item_box->setStyle(rhea::StylePropertyId::kBackgroundColor, "rgb(0.2 0.2 0.3 1.0)");
			}
			else {
                item_box->setup(this, {}, {
               {"w", Variant(280.0f)},
               {"h", Variant(60.0f)},
                {"texture", "//sandbox/grass256.png"},
                    });
				item_box->setStyle(rhea::StylePropertyId::kBackgroundColor, "rgb(0.3 0.3 0.4 1.0)");
			}
			
			// Set different background colors for each item
			float hue = (i % 3) * 0.1f;
			std::string bg_color = "rgb(" + std::to_string(0.3f + hue) + " 0.4 0.5 1.0)";
		//	item_box->setStyle(rhea::StylePropertyId::kBackgroundColor, bg_color);
			item_box->setStyle(rhea::StylePropertyId::kPadding, "5");
				column->addChild(item_box);

				// Optional: Add a colored marker box instead of text for now
				rhea::Widget* marker = simpleparser::Type::create<rhea::Widget>("marker_" + std::to_string(i));
				marker->setup(this, {}, {
					{"x", Variant(10.0f)},
					{"y", Variant(20.0f)},
					{"w", Variant(260.0f)},
					{"h", Variant(20.0f)},
                    {"texture", "//sandbox/kies.png"},
				});
				// Different color per item for identification
				float r = (i % 5) * 0.2f;
				std::string marker_color = "rgb(" + std::to_string(0.5f + r) + " 0.7 0.3 1.0)";
				marker->setStyle(rhea::StylePropertyId::kBackgroundColor, marker_color);
				item_box->addChild(marker);

				/* DISABLED TEXT UNTIL FONT IS PROPERLY LOADED
				// Create Label for item text
				rhea::Label* label = simpleparser::Type::create<rhea::Label>("item_label_" + std::to_string(i));
				label->setup(this, {}, {
					{"text", Variant("ScrollView Test Item #" + std::to_string(i + 1))},
					{"x", Variant(10.0f)},
					{"y", Variant(10.0f)}
				});
				// Set text color and font size
				label->setStyle(rhea::StylePropertyId::kColor, "rgb(1.0 1.0 1.0 1.0)");
				label->setStyle(rhea::StylePropertyId::kFontSize, "16");
				item_box->addChild(label);

				// Add Button to test interaction
				rhea::Button* button = simpleparser::Type::create<rhea::Button>("item_button_" + std::to_string(i));
				button->setup(this, {}, {
					{"x", Variant(10.0f)},
					{"y", Variant(35.0f)},
					{"width", Variant(100.0f)},
					{"height", Variant(20.0f)}
				});
				button->setText("Button " + std::to_string(i + 1));
				// Set button background, text color and font size
				button->setStyle(rhea::StylePropertyId::kBackgroundColor, "rgb(0.5 0.5 0.6 1.0)");
				button->setStyle(rhea::StylePropertyId::kFontSize, "14");
				item_box->addChild(button);
				*/
			}

		// Ready the GUI
		m_gui_controller->ready();
		m_gui_controller->invalidateLayout();
        m_gui_controller->UpdateStyle();

		// Show the GUI controller (makes it visible)
		m_gui_controller->show();

		// Trigger viewport size event to layout GUI
		rhea::SizeEvent size_event(m_gui_controller, 800, 600);
		m_gui_controller->sendEvent("viewport_size", &size_event);

		LOG(LOG_INFO) << "ScrollView GUI test setup complete";
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

        if (m_gizmo && m_cube) {
            rhea::Position pos = *m_cube->getTransform()->get_derived_position();
            math::Matrix3x4 m = pos.getWorldTransform();
            rhea::LineBatchMesh* lineMesh = doCast<rhea::LineBatchMesh>(m_gizmo->getComponent<rhea::MeshComponent>()->getMesh());
            lineMesh->clearLines();
            lineMesh->addAxis(m, 1.0f);
            lineMesh->update();
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