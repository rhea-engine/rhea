#include "Poseidon/Core.h"
#include "Poseidon/Mesh.h"
#include "Poseidon/Texture.h"
#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include "Poseidon/Node.h"
#include "Poseidon/Entity.h"
#include "Rhea/Dispatcher.h"
#include "Rhea/Scene.h"
#include "Rhea/Graphics/GraphicsService.h"
#include "Rhea/Physics/PhysicsService.h"
#include "Rhea/Physics/PhysicsManager.h"

#include <windows.h>
#include <stdint.h>

#include "Components/Psio/Keyboard.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/Math/Matrix3x4.h"
#include "Poseidon/Math/Matrix4.h"
#include "Poseidon/Texture.h"

#define USE_WVP0

#include "Cube.h"

using Hippios::vertex_uv;

using Poseidon::Viewport;

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);

void CubeApp::setup()
{		
	m_services["physics"] = new Rhea::PhysicsService(this);

	Rhea::Application::setup();

	m_scene = core->get_scene_manager();
	m_material = core->createMaterial();
	Poseidon::Shader* shader = core->createShader("uvshader_vp");
#ifdef USE_WVP
	if (shader->loadFromFile("rhea\\shaders\\uvshader.hlsl", "VShader", "PShader"))
		exit(-1);
#else
	if (shader->loadFromFile("rhea\\shaders\\uvshader_vp.hlsl", "VShader", "PShader"))
		exit(-1);
#endif
	m_material->setShader(shader);
	core->registerMaterial("default", m_material);

	initialize();
}

void CubeApp::initialize()
{
	Viewport* viewport = core->get_scene_manager()->getViewport();
	
	Rhea::Scene* scene = new Rhea::Scene("cubescene");
	static_cast<Rhea::GraphicsService*>(m_services["graphics"])->setScene(scene);
	static_cast<Rhea::PhysicsService*>(m_services["physics"])->setScene(scene);

	const int num_faces = 12;
	const int num_vertices = 24;

	Hippios::vertex_uv OurVertices[num_vertices] = {
		// Front Face
		vertex_uv(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		vertex_uv(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		vertex_uv(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		vertex_uv(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Back Face
		vertex_uv(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
		vertex_uv(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		vertex_uv(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
		vertex_uv(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

		// Top Face
		vertex_uv(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
		vertex_uv(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
		vertex_uv(1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
		vertex_uv(1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

		// Bottom Face
		vertex_uv(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
		vertex_uv(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		vertex_uv(1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
		vertex_uv(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

		// Left Face
		vertex_uv(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
		vertex_uv(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
		vertex_uv(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		vertex_uv(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Right Face
		vertex_uv(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		vertex_uv(1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		vertex_uv(1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
		vertex_uv(1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
	};
/*	int i = 0;
	for (int i = 0; i < num_vertices; i++)
	{
		vertices.push_back(OurVertices[i++]);
	}
	*/

	mesh = new Poseidon::Mesh();
	mesh->setViewport(viewport);
	mesh->setVertices(sizeof(Hippios::vertex_uv), num_vertices, OurVertices);

	unsigned long indices[] = {
		// Front Face
		0, 1, 2,
		0, 2, 3,

		// Back Face
		4, 5, 6,
		4, 6, 7,

		// Top Face
		8, 9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	mesh->setIndices(sizeof(unsigned long), num_faces, indices);

	// load texture
	Poseidon::Texture* tex;
	tex = core->createTexture("Samples\\assets\\images\\kies.png");
	if (!tex) {
		printf("Loading of texture failed\n");
		exit(-1);
	}
	m_material->addTexture(tex);

	//renderer = engine->getRenderer();
	//SwapChain = renderer->get_swapchain();

	//renderTargetView = renderer->get_backbuffer();
	//depthStencilView = renderer->get_depth_buffer()->get_view();

	// add entities
	cube1 = core->createEntity(mesh);
	m_scene->getViewport()->addRenderChild(cube1);
	
	Poseidon::Position* pos = cube1->get_position();
	pos->setPosition(0.0f, 0.0f, 4.0f);

	cube1->setMaterial(m_material);

	cube2 = core->createEntity(mesh);
	m_scene->getViewport()->addRenderChild(cube2);
	pos = cube2->get_position();
	pos->setPosition(0.0f, 1.0f, 0.0f);
	cube2->setMaterial(m_material);

	// add a camera
	m_camera = core->createCamera();
	m_scene->getViewport()->addChild(m_camera);
	m_scene->getViewport()->setCamera(m_camera);

	m_position = Poseidon::Vector3(0.0f, 3.0f, -100.0f);

	// set a default position
	m_camera->get_position()->setPosition(m_position);
	m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);

	m_camera->setAspectRatio((float)Width / (float)Height);

	// setup event handlers
	e_left = new Psio::KeyInputEvent(Psio::InputEventId::KEY_PRESSED);
	e_left->set(Psio::Key::CODE_LEFT, 0, 0);
	e_right = new Psio::KeyInputEvent(Psio::InputEventId::KEY_PRESSED);
	e_right->set(Psio::Key::CODE_RIGHT, 0, 0);
	e_up = new Psio::KeyInputEvent(Psio::InputEventId::KEY_PRESSED);
	e_up->set(Psio::Key::CODE_UP, 0, 0);
	e_down = new Psio::KeyInputEvent(Psio::InputEventId::KEY_PRESSED);
	e_down->set(Psio::Key::CODE_DOWN, 0, 0);
	e_home = new Psio::KeyInputEvent(Psio::InputEventId::KEY_PRESSED);
	e_home->set(Psio::Key::CODE_HOME, 0, 0);
	e_end = new Psio::KeyInputEvent(Psio::InputEventId::KEY_PRESSED);
	e_end->set(Psio::Key::CODE_END, 0, 0);

	std::shared_ptr<Psio::InputEvent> evt = std::make_shared<Psio::MouseInputEvent>(Psio::InputEventId::MOUSE_BTN_PRESSED);
	std::static_pointer_cast<Psio::MouseInputEvent>(evt)->setBtn(true, false, false);
	m_input_map.setAction(evt, "left_mouse");
	m_input_map.setAction(evt, "left mouse - really");

	getDispatcher()->subscribeEvent("move_right", [&](std::string act, std::shared_ptr<Psio::InputEvent> evt) -> bool {
		log("move right");
		return false;
		});
}

int main()
{
	CubeApp* app = new CubeApp();

	app->init();
	app->setup();

	// start the loop
	int ret = app->loop();

	app->close();

	return ret;
}

bool CubeApp::onFrameBegin()
{
	// run game code            
	UpdateScene();
	core->iterate();

	return false;
}

bool CubeApp::onInput(std::shared_ptr<Psio::InputEvent> evt)
{
	if (evt->isEvent(e_left)) {
		m_position.x -= 0.1f;
		m_camera->get_position()->setPosition(m_position);
		m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
	}
	else if (evt->isEvent(e_right)) {
		m_position.x += 0.1f;
		m_camera->get_position()->setPosition(m_position);
		m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
	}
	else if (evt->isEvent(e_up)) {
		m_position.z += 0.5f;
		m_camera->get_position()->setPosition(m_position);
		m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
	}
	else if (evt->isEvent(e_down)) {
		m_position.z -= 0.5f;
		m_camera->get_position()->setPosition(m_position);
		m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
	}
	else if (evt->isEvent(e_home)) {
		m_position.y += 0.5f;
		m_camera->get_position()->setPosition(m_position);
		m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
	}
	else if (evt->isEvent(e_end)) {
		m_position.y -= 0.5f;
		m_camera->get_position()->setPosition(m_position);
		m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
	}
	else {
		Rhea::ievt_actions iea = m_input_map.getActions(evt);
		for (auto it = iea.actions.begin(); it < iea.actions.end(); ++it)
			log(*it);
		
		return Application::onInput(evt);		
	}
	return false;
}

void CubeApp::UpdateScene()
{
	//Keep the cubes rotating
	rot += .0005f;
	if (rot > 6.26f)
		rot = 0.0f;

	Poseidon::Position* pos = cube1->get_position();
	pos->setPosition(0, 0, 0);
	pos->setOrientation(Poseidon::Quaternion(rot, Poseidon::Vector3(0.0f, 1.0f, 0.0f)));
	pos->moveRelative(Poseidon::Vector3(4.0f,0.0f,0.0f));

	pos = cube2->get_position();
	pos->setPosition(0, 0, 0);
	pos->setOrientation(Poseidon::Quaternion(-rot, Poseidon::Vector3(0.0f, 1.0f, 0.0f)));
}
