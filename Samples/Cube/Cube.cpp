#include "Poseidon/Core.h"
#include "Poseidon/Mesh.h"
#include "Poseidon/Texture.h"
#include "Poseidon/Material.h"
#include "Poseidon/Node.h"
#include "Poseidon/Entity.h"

#include <windows.h>
#include <stdint.h>

#include "Components/Psio/Psio.h"
#include "Components/Psio/Win32Window.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/Math/Matrix3x4.h"
#include "Poseidon/Math/Matrix4.h"
#include "Poseidon/Texture.h"

#define USE_WVP0

#include "Cube.h"

using Hippios::vertex_uv;

using Poseidon::Viewport;

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);

void CubeApp::setup()
{
	Rhea::Application::setup();

	m_scene = core->get_scene_manager();
	Poseidon::Material* material = core->createMaterial();
	material->setType(1);
#ifdef USE_WVP
	if (material->loadFromFile("Samples\\assets\\shaders\\uvshader.hlsl", "vs", "VShader"))
		exit(-1);
	if (material->loadFromFile("Samples\\assets\\shaders\\uvshader.hlsl", "ps", "PShader"))
		exit(-1);
#else
	if (material->loadFromFile("Samples\\assets\\shaders\\uvshader_vp.hlsl", "vs", "VShader"))
		exit(-1);
	if (material->loadFromFile("Samples\\assets\\shaders\\uvshader_vp.hlsl", "ps", "PShader"))
		exit(-1);
	material->initBuffer(sizeof(cbPerObjectMVP));
#endif

	initialize();
}

void CubeApp::initialize()
{
	Viewport* viewport = core->get_scene_manager()->getViewport();
	//engine = viewport->get_renderer();

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
	mesh->addTexture(tex);

	//renderer = engine->getRenderer();
	//SwapChain = renderer->get_swapchain();

	//renderTargetView = renderer->get_backbuffer();
	//depthStencilView = renderer->get_depth_buffer()->get_view();

	// add entities
	cube1 = core->createEntity(mesh);
	m_scene->getViewport()->addRenderChild(cube1);
	
	Poseidon::Position* pos = cube1->get_position();
	pos->setPosition(0.0f, 0.0f, 4.0f);

	cube2 = core->createEntity(mesh);
	m_scene->getViewport()->addRenderChild(cube2);
	pos = cube2->get_position();
	pos->setPosition(0.0f, 1.0f, 0.0f);

	// add a camera
	m_camera = core->createCamera();
	m_scene->getViewport()->addChild(m_camera);
	m_scene->getViewport()->setCamera(m_camera);

	m_position = Poseidon::Vector3(0.0f, 3.0f, -100.0f);

	// set a default position
	m_camera->get_position()->setPosition(m_position);
	m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);

	m_camera->setAspectRatio((float)Width / (float)Height);
}

int main0()
{
	CubeApp* app = new CubeApp();

	app->init();
	app->setup();

	// start the loop
	int ret = app->loop();

	app->close();

	return ret;
}

int main()
{
	CubeApp* app = new CubeApp();

	app->init();
	app->setup();

	app->messageloop();

	app->CleanUp();

	app->close();

	return 0;
}

void CubeApp::CleanUp()
{
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

int CubeApp::messageloop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		BOOL PeekMessageL(
			LPMSG lpMsg,
			HWND hWnd,
			UINT wMsgFilterMin,
			UINT wMsgFilterMax,
			UINT wRemoveMsg
		);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.wParam)
				{
				case VK_LEFT:
					m_position.x -= 0.1f;
					m_camera->get_position()->setPosition(m_position);
					m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
					break;
				case VK_RIGHT:
					m_position.x += 0.1f;
					m_camera->get_position()->setPosition(m_position);
					m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
					break;
				case VK_UP:
					m_position.z += 0.5f;
					m_camera->get_position()->setPosition(m_position);
					m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
					break;
				case VK_DOWN:
					m_position.z -= 0.5f;
					m_camera->get_position()->setPosition(m_position);
					m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
					break;
				case VK_HOME:
					m_position.y += 0.5f;
					m_camera->get_position()->setPosition(m_position);
					m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
					break;
				case VK_END:
					m_position.y -= 0.5f;
					m_camera->get_position()->setPosition(m_position);
					m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);
					break;

				}

			}
		}
		else {
		
			// run game code            
			UpdateScene();
			core->iterate();
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}
