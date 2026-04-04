#include "Poseidon/Core.h"
#include "Poseidon/Mesh.h"
#include <Psio/Image/Image.h>
#include "Poseidon/Material.h"
#include "Poseidon/Viewport.h"

#define USE_INDICES 1

#include "Triangle.h"

using Poseidon::Viewport;

void TriangleApp::setup()
{
	rhea::Application::setup();

	m_scene = core->get_scene_manager();	
	Poseidon::Material* material = core->createMaterial();
	material->loadFromFile("Data\\Rhea\\shaders\\shader.hlsl", "vs", "VShader");
	material->loadFromFile("Data\\Rhea\\shaders\\shader.hlsl", "ps", "PShader");
	
	initialize();
}

/** initalie app
*	Draw two colorized triangles making a quad
*/

void TriangleApp::initialize()
{
	Poseidon::Mesh* mesh;
	unsigned long* indices;
	Viewport* viewport = m_scene->getViewport();

	//vertices.resize(10);
	float colorA[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float colorB[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float colorC[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

	const int num_vertices = 4;
	const int num_faces = 2;

	Hippios::vertex OurVertices[num_vertices] = {
/*{0.0f, 0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f }},
{0.45f, -0.5, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f }},
{-0.45f, -0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f }}*/
		{-0.5f, -0.5f, 0.5f, { 1.0f, 0.0f, 0.0f, 1.0f }},
		{-0.5f, 0.5f, 0.5f, { 0.0f, 1.0f, 0.0f, 1.0f }},
		{0.5f, 0.5f, 0.5f, { 0.0f, 1.0f, 0.0f, 1.0f }},
		{0.75f, -0.75f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f }}
	};

	for (int i = 0; i < num_vertices; i++)
		vertices.push_back(OurVertices[i]);

	mesh = new Poseidon::Mesh();
	mesh->setViewport(viewport);
	mesh->setVertices(sizeof(Hippios::vertex), num_vertices, vertices.data());

#ifdef USE_INDICES
	// Load the index array with data.
		indices = new unsigned long[num_faces * 3]; // 3: a triangle has three points
	indices[0] = 0;  // Bottom left.
	indices[1] = 2;  // Top middle.
	indices[2] = 3;  // Bottom right.

	indices[3] = 0;  
	indices[4] = 1; 
	indices[5] = 2; 

	mesh->setIndices(sizeof(unsigned long), num_faces, indices);
#endif // USE_INDICES	
}

int main()
{
	TriangleApp* app = new TriangleApp();

	app->init();	

	// start the loop
	int ret = app->loop();
	
	app->close();

	return ret;
}
