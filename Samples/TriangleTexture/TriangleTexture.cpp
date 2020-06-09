#include "Poseidon/Core.h"
#include "Poseidon/Mesh.h"
#include "Poseidon/Texture.h"
#include "Poseidon/Material.h"
#include "Poseidon/SceneManager.h"

#include "TriangleTexture.h"

using Hippios::vertex_uv;
using Poseidon::Viewport;

void TriangleTextureApp::setup()
{
	Rhea::Application::setup();

	m_scene = core->get_scene_manager();
	Poseidon::Material* material = core->createMaterial();
	material->setType(1);
	if (material->loadFromFile("Data\\Rhea\\shaders\\uvshader.hlsl", "vs", "VShader"))
		exit(-1);
	if (material->loadFromFile("Data\\Rhea\\shaders\\uvshader.hlsl", "ps", "PShader"))
		exit(-1);

	initialize();
}

void TriangleTextureApp::initialize()
{
	Poseidon::Mesh* mesh;	

	const int num_faces = 12;
	const int num_vertices = 24;
	Viewport* viewport = core->get_scene_manager()->getViewport();

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
/*		{0.0f, 0.8f, 0.5f,   0.5f, 1.0f},
		{-0.8f,-0.8f, 0.5f,   0.0f, 0.0f},
		{0.8f,-0.8f, 0.5f,   1.0f, 0.0f}*/

	};
	int i = 0;
	for(int i = 0; i < num_vertices; i++)
	{
		vertices.push_back(OurVertices[i++]);
	}

	mesh = new Poseidon::Mesh();
	mesh->setViewport(viewport);
	mesh->setVertices(sizeof(Hippios::vertex_uv), num_vertices, OurVertices);	

	
	// Load the index array with data.
	/*indices = new unsigned long[num_faces * 3];
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.*/

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
	mesh->addTexture(tex);
}

int main()
{
	TriangleTextureApp* app = new TriangleTextureApp();

	app->init();

	// start the loop
	int ret = app->loop();

	app->close();

	return ret;
}
